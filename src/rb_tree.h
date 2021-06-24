#ifndef _RB_TREE_H
#define _RB_TREE_H

#include "iterator.h"
#include "iterator_base.h"
#include "allocator.h"
#include "functional.h"


namespace toystl {
    // rb_tree 的节点颜色的类型
    using rb_tree_color_type = bool;
    static constexpr rb_tree_color_type rb_tree_red = false;
    static constexpr rb_tree_color_type rb_tree_black = true;

    struct rb_tree_node_base
    {
        using color_type = rb_tree_color_type;
        using base_ptr   = rb_tree_node_base*;
        
        color_type color;  // 节点颜色
        base_ptr parent;   // 节点的父节点
        base_ptr left;     // 节点的左子节点
        base_ptr right;    // 节点的右子节点

        static base_ptr s_minimum(base_ptr x) {
            while(x->left != nullptr) {
                x = x->left;
            }

            return x;
        }

        static base_ptr s_maximum(base_ptr x) {
            while(x->right != nullptr) {
                x = x->right;
            }

            return x;
        }
    };

    // // 红黑树的 header 节点
    // struct rb_tree_header 
    // {
    //     rb_tree_node_base header;
    //     size_t node_count;

    //     rb_tree_header()
    //     {
    //         header.color = rb_tree_red;
    //         reset();
    //     }

    //     rb_tree_header(rb_tree_header&& x) noexcept
    //     {
    //         if(header.parent != nullptr) {
    //             move_data(x);
    //         }
    //         else {
    //             header.color = rb_tree_red;
    //             reset();
    //         }
    //     }

    //     void move_data(rb_tree_header& from)
    //     {
    //         header.color = from.header.color;
    //         header.parent = from.header.parent;
    //         header.left = from.header.left;
    //         header.right = from.header.right;
    //         header.parent->parent = & header;
    //         node_count = from.node_count;
    //     }

    //     void reset()
    //     {
    //         header.parent = 0;
    //         header.left = &header;
    //         header.right = &header;
    //         node_count = 0;
    //     }
    // };

    template <class Value>
    struct rb_tree_node : public rb_tree_node_base
    {
        using link_type = rb_tree_node<Value>*;
        Value value_field;
    };

    // 基层迭代器
    struct rb_tree_base_iterator
    {
        using base_ptr              = typename rb_tree_node_base::base_ptr;
        using iterator_category     = bidirectional_iterator_tag;
        using difference_type       = ptrdiff_t;
        
        base_ptr node; // 它用来与容器之间产生一个连接关系

        void increment()
        {
            if(node -> right != nullptr) {      // 如果有右子节点。状况（1）
                node = node->right;             // 就往右走一步

                while(node->left != nullptr) {  // 然后一直往左子树走到底
                    node = node->left;          // 此时即为答案
                }
            }
            else {                              // 没有右子节点。状况（2）
                base_ptr y = node->parent;      // 找出 父节点
                while(node == y->right) {       // 如果现行节点本身是一个右子节点
                    node = y;                   // 就一直上溯，直到 "不为右子节点"为止
                    y = y->parent;
                }

                if(node->right != y) {          // 若此时的右子节点不等于此时的父节点
                    node = y;                   // 状况（2.3），此时的父节点就是答案
                }
                                                // 否则此时的 node 为解答，状况（2.4）
                // 状况（2.4）是为了应付一种情况：
                // 我们欲寻根节点的下一个节点，而恰巧根节点无右子节点
                // 当然以上特殊做法需要配合 rb_tree 根节点与特殊之 header 之间的特殊关系
            }
        }

        void decrement()
        {
            if(node->color == rb_tree_red && node->parent->parent == node) {
                node = node -> right;
            }
            else if(node->left != nullptr) {
                base_ptr y = node->left;
                while(y->right != nullptr) {
                    y = y->right;
                } 
                node = y;
            }
            else {                              // 既非根节点，也没有左子节点
                base_ptr y = node->parent;
                while(node == y->left) {
                    node = y;
                    y = y->parent;
                }
                node = y;
            }
        }
    };

    // 正规迭代器
    template <class Value, class Ref, class Ptr>
    struct rb_tree_iterator : public rb_tree_base_iterator
    {
        using value_type      = Value;
        using reference       = Ref;
        using pointer         = Ptr;
        using iterator        = rb_tree_iterator<Value, Value&, Value*>;
        using const_iterator  = rb_tree_iterator<Value, const Value&, const Value*>;
        using self            = rb_tree_iterator<Value, Ref, Ptr>;
        using Link_type       = rb_tree_node<Value>*;

        rb_tree_iterator() {}
        rb_tree_iterator(Link_type x) { node = x; }
        rb_tree_iterator(const iterator& it) { node = it.node; }

        reference operator*() const { return Link_type(node)->value_field; }
        pointer operator->() const { return &(operator*()); }

        self& operator++() { increment(); return *this;}
        self operator++(int) {
            self tmp = *this;
            increment();
            return tmp;
        }

        self& operator--() { decrement(); return *this;}
        self operator--(int) {
            self tmp = *this;
            decrement();
            return tmp;
        }
    };

    inline bool operator==(const rb_tree_base_iterator& x,
                           const rb_tree_base_iterator& y)
    {
        return x.node == y.node;
    }
    
    inline bool operator!=(const rb_tree_base_iterator& x,
                           const rb_tree_base_iterator& y)
    {
        return x.node != y.node;
    }

    /*---------------------------------------*\
    |       p                         p       |
    |      / \                       / \      |
    |     x   d    rotate left      y   d     |
    |    / \       ===========>    / \        |
    |   a   y                     x   c       |
    |      / \                   / \          |
    |     b   c                 a   b         |
    \*---------------------------------------*/    
    // x 为左旋点，root 为根节点
    inline void rb_tree_rotate_left(rb_tree_node_base* x,
                                    rb_tree_node_base*& root)
    {
        auto y = x->right;          // y 为旋转点的右子节点
        x->right = y->left;
        if(y->left != nullptr) {
            y->left->parent = x;
        }
        y->parent = x->parent;

        if(x == root) {                  // 如果 x 为根节点，让 y 顶替 x 成为根节点
            root = y;
        }
        else if(x == x->parent->left) {  // 如果 x 是左子节点
            x->parent->left = y;
        }
        else {                           // 如果 x 是右子节点
            x->parent->right = y;
        }
        y->left = x;                     // 调整 x 与 y 之间的联系
        x->parent = y;
    }

    /*----------------------------------------*\
    |     p                         p          |
    |    / \                       / \         |
    |   d   x      rotate right   d   y        |
    |      / \     ===========>      / \       |
    |     y   a                     b   x      |
    |    / \                           / \     |
    |   b   c                         c   a    |
    \*----------------------------------------*/
    inline void rb_tree_rotate_right(rb_tree_node_base* x,
                                     rb_tree_node_base*& root)
    {
        auto y = x->left;
        x->left = y->right;
        if(y->right) {
            y->right->parent = x;
        }

        y->parent = x->parent;

        if(x == root) {                   // x 为根节点，让 y 顶替 x 成为根节点
            root = y;
        }
        else if(x->parent->right == x) {  // x 为右子节点
            x->parent->right = y;
        }
        else {                            // x 为左子节点
            x->parent->left = y;
        }
        // 调整 y 与 x 之间的关系
        y->right = x;
        x->parent = y;
    }

    // 插入节点后
    // 使 rb_tree 重新平衡，x 为新增节点，root 为根节点
    // case1：新增节点位于根节点，令新增节点为黑
    // case2：新增节点的父节点为黑，没有破坏平衡，直接返回
    // case3：父节点和伯父节点都为红，令父节点和伯父节点都为黑，祖父节点为红，但是此时可能导致双红现象上溯，此时需要考察祖父节点
    // case4：父节点为红，叔叔节点为 NIL 或黑色，父节点为左（右）子节点，当前节点为右（左）子节点，
    //        让父节点成为当前节点，再以当前节点为支点左（右）旋
    // case5：父节点为红，叔叔节点为 NIL 或黑色，父节点为左（右）孩子，当前节点为左（右）孩子，
    //        让父节点变为黑色，祖父节点变为红色，以祖父节点为支点右（左）旋
    inline void rb_tree_rebalance(rb_tree_node_base* x,
                                  rb_tree_node_base*& root)
    {
        x->color = rb_tree_red; // 新增节点为红色
        while(x != root && x->parent->color == rb_tree_red) {  // 父节点为红
            if(x->parent == x->parent->parent->left) {  // 父节点为祖父节点的左子节点
                rb_tree_node_base* y = x->parent->parent->right;  // 令 y 为伯父节点
                if(y && y->color == rb_tree_red) {                // 伯父节点存在且为红
                    // case3：父节点和伯父节点都是红色
                    x->parent->color = rb_tree_black;             // 更改父节点为黑
                    y->color = rb_tree_black;                     // 更改伯父节点为黑
                    x->parent->parent->color = rb_tree_red;       // 更改祖父节点为红 
                    x = x->parent->parent;                        // 但是此时祖父节点为红的话，祖父节点的父亲可能也是红，可能导致 "双红现象" 上溯
                }
                else {                                            // 无叔叔节点或叔叔节点为黑
                    if(x->parent->right == x) {                   // 如果当前节点为右子节点
                        // case4：父节点为红，叔叔节点为 NIL 或黑色，父节点为左子节点，当前节点为右子节点
                        x = x->parent;
                        rb_tree_rotate_left(x, root);             // 左旋
                    }
                    // 都转换成了 case5：当前结点为左子节点
                    x->parent->color = rb_tree_black;
                    x->parent->parent->color = rb_tree_red;
                    rb_tree_rotate_right(x->parent->parent, root);
                    break;
                }
            }
            else {                                                // 如果父节点为右子节点，对称处理
                rb_tree_node_base* y = x->parent->parent->left;   // 令 y 为伯父节点
                if(y && y->color == rb_tree_red) {                // 伯父节点存在且为红
                    // case3：父节点和伯父节点都为红
                    x->parent->color = rb_tree_black;
                    y->color = rb_tree_black;
                    x->parent->parent->color = rb_tree_red;
                    x = x->parent->parent;
                }
                else {                                            // 无叔叔节点或者叔叔节点为黑
                    if(x->parent->left == x) {                    // 如果当前节点为左子节点
                    // case4：父节点为红，叔叔节点为 NIL 或黑色，父节点为右子节点，当前节点为左子节点
                        x = x->parent;
                        rb_tree_rotate_right(x, root);   
                    }
                    // 都转换成了 case5：当前节点为右子节点
                    x->parent->color = rb_tree_black;
                    x->parent->parent->color = rb_tree_red;
                    rb_tree_rotate_left(x->parent->parent, root);
                    break;
                }
            }
        } // while

        root->color = rb_tree_black;    // 根节点永远为黑
    }
    

    // 参考：https://blog.csdn.net/v_JULY_v/article/details/6105630
    // 参考：https://github.com/Alinshans/MyTinySTL/blob/master/MyTinySTL/rb_tree.h
    // 删除节点后
    // 使 rb_tree 平衡，z 为要删除的节点，root 为根节点，leftmost 为最小的节点，rightmost 为最大的节点
    inline rb_tree_node_base* rb_tree_rebalance_for_erase(rb_tree_node_base* z,
                                            rb_tree_node_base*& root,
                                            rb_tree_node_base*& leftmost,
                                            rb_tree_node_base*& rightmost)
    {   
        // y 是可能替换节点，指向最终要删除的节点

        // 如果 z 的两个子节点都为非空，找到 z 的直接后继节点
        rb_tree_node_base* z_next;
        if(z->left && z->right) {
            if(z->right != nullptr) {
                z_next = z->right;
                while(z_next->left) {
                    z_next = z_next->left;
                }
            }
        }
        rb_tree_node_base* y = (z->left == nullptr || z->right == nullptr) ? z : z_next;
        // x 是 y 的一个独子节点或 NIL节点
        rb_tree_node_base* x = (y->left != nullptr) ? y->left : y->right;
        rb_tree_node_base* x_parent = nullptr;

        // y != z 说明 z 有两个非空的子节点，此时 y 指向 z 右子树的最左节点，x 指向 y 的右子节点
        // 用 y 顶替 z 的位置，用 x 顶替 y 的位置，最后 用 y 指向 z
        if(y != z) {
            z->left->parent = y;
            y->left = z->left;

            // 如果 y 不是 z 的右子节点，那么 z 的右子节点一定有左孩子
            if(y != z->right) {
                // x 替换 y 的位置
                x_parent = y->parent;
                if(x != nullptr) {
                    x->parent = y->parent;
                }
                y->parent->left = x;

                // 调整 y 和 z 的位置
                y->right = z->right; 
                z->right->parent = y;
            }
            else {
                x_parent = x;
            }

            // 连接 y 与 z 的父节点
            if(root == z) {
                root = y;
            }
            else if(z->parent->left == z) {
                z->parent->left = y;
            }
            else {
                z->parent->right = y;
            }
            y->parent = z->parent;
            toystl::swap(y->color, z->color);
            
            y = z;
        }
        // y == z 说明 z 至多只有一个孩子
        // 对应两种情况：
        // 1、z 没有孩子。直接将 z 的 父节点对应的儿子指针设为 NULL，删除 z 节点即可。
        // 2、z 只有一个孩子。那么将 z 父节点的相应儿子指针指向 z 的独生子，删除 z 节点即可。
        else {
            x_parent = y->parent;
            if(x) {
                x->parent = y->parent;
            }

            // 连接 x 与 z 的父节点
            if(root == z) {
                root = x;
            }
            else if(z->parent->left == z) {
                z->parent->left = x;
            }
            else if(z->parent->right == z) {
                z->parent->right = x;
            }

            // 此时 z 有可能是最左节点或者最右节点，更新数据
            if(leftmost == z) {
                leftmost = x == nullptr ? x_parent : rb_tree_node_base::s_minimum(x);
            }
            if(rightmost == z) {
                rightmost = x == nullptr ? x_parent : rb_tree_node_base::s_maximum(x);
            }
        }

        // 此时 y 指向要被删除的节点，x 为替代节点，从 x 节点开始调整
        // 如果删除的节点为红色，树的性质没有被破坏，否则按照下面情况进行调整（以 x 为左子节点为例）：

        // 以下的情况，删除的节点为黑色。双黑修正。
        // case1 ：兄弟节点为红色，令父节点为红，兄弟节点为黑，进行左（右）旋，继续处理
        // case2 ：兄弟节点为黑色，且两个子节点都为黑色或者 NIL，令兄弟父节点为红，父节点成为当前节点，继续处理
        // case3 ：兄弟节点为黑色，左子节点为红色或者 NIL，右子节点为黑色或 NIL
        //         令兄弟节点为红，兄弟节点的左子节点为黑，以兄弟节点为支点右（旋），继续处理
        // case4 ：兄弟节点为黑色，两个右子节点都为红色
        //         令兄弟节点为父节点的颜色，父节点为黑色，兄弟节点的右子节点为黑色，以父节点为支点左（右）旋，树的性质调整完毕，算法结束
        if(y->color != rb_tree_red) {
            while(x != root && (x == nullptr || x->color == rb_tree_black)) {
                if(x == x_parent->left) {  // x 为左子节点
                    rb_tree_node_base* x_brother = x_parent->right;
                    if(x_brother->color == rb_tree_red) {
                        // case1：兄弟节点为红色
                        x_brother->color = rb_tree_black;
                        x_parent->color = rb_tree_red;
                        rb_tree_rotate_left(x_parent, root);
                        x_brother = x_parent->right;
                    }
                    // case1 转换成了 case2\3\4 中的一种
                    if((x_brother->left == nullptr || x_brother->left->color == rb_tree_black) && 
                      (x_brother->right == nullptr || x_brother->right->color == rb_tree_black))
                    {   
                        // case2：兄弟节点为黑，且两个子节点都为黑或者 NIL
                        x_brother->color = rb_tree_red;
                        x = x_parent;
                        x_parent = x_parent->parent;
                    } else {
                        if(x_brother->right == nullptr || x_brother->right->color != rb_tree_red) {
                            // case3：兄弟节点为黑色，左子节点为红色或者 NIL，右子节点为黑色或 NIL
                            if(x_brother->left != nullptr) {
                                x_brother->left->color = rb_tree_black;
                            }
                            x_brother->color = rb_tree_red;
                            rb_tree_rotate_right(x_brother, root);
                            x_brother = x_parent->right;
                        }
                        // 转为 case4：
                        x_brother->color = x_parent->color;
                        x_parent->color = rb_tree_black;
                        if(x_brother->right != nullptr) {
                            x_brother->right->color = rb_tree_black;
                        }
                        rb_tree_rotate_left(x_parent, root);
                        break;
                    }
                }
                else {                                           // same as above, with right <-> left
                    rb_tree_node_base* x_brother = x_parent->left;
                    if(x_brother->color == rb_tree_red) {
                        // case1：兄弟节点为红色
                        x_brother->color = rb_tree_black;
                        x_parent->color = rb_tree_red;
                        rb_tree_rotate_right(x_parent, root);
                        x_brother = x_parent->left;
                    }
                    // case1 转换成了 case2\3\4 中的一种
                    if((x_brother->right == nullptr || x_brother->right->color == rb_tree_black) && 
                      (x_brother->left == nullptr || x_brother->left->color == rb_tree_black))
                    {   
                        // case2：兄弟节点为黑，且两个子节点都为黑或者 NIL
                        x_brother->color = rb_tree_red;
                        x = x_parent;
                        x_parent = x_parent->parent;
                    } else {
                        if(x_brother->left == nullptr || x_brother->right->color != rb_tree_red) {
                            // case3：兄弟节点为黑色，左子节点为红色或者 NIL，右子节点为黑色或 NIL
                            if(x_brother->right != nullptr) {
                                x_brother->right->color = rb_tree_black;
                            }
                            x_brother->color = rb_tree_red;
                            rb_tree_rotate_left(x_brother, root);
                            x_brother = x_parent->left;
                        }
                        // 转为 case4：
                        x_brother->color = x_parent->color;
                        x_parent->color = rb_tree_black;
                        if(x_brother->left != nullptr) {
                            x_brother->left->color = rb_tree_black;
                        }
                        rb_tree_rotate_right(x_parent, root);
                        break;
                    }
                }
            }
            if(x) {
                x->color = rb_tree_black;
            }
        }

        return y;
    }


    // rb_tree
    template <class Key,
              class Value,
              class KeyOfValue,     // 怎么从 value 取出 key
              class Compare,        // 键值大小比较规则
              class Allocator = toystl::allocator<Value>>
    class rb_tree
    {
    protected:
        using base_ptr          = rb_tree_node_base*;
        using Node              = rb_tree_node<Value>;
        using nodeAllocator     = typename Allocator::template rebind<Node>::other;
        using dataAllocator     = Allocator;

    public:
        using key_type          = Key;
        using value_type        = Value;
        using pointer           = value_type*;
        using const_pointer     = const value_type*;
        using reference         = value_type&;
        using const_reference   = const value_type&;
        using link_type         = Node*;
        using size_type         = size_t;
        using difference_type   = ptrdiff_t;
        
        using iterator                  = rb_tree_iterator<value_type, reference, pointer>;
        using const_iterator            = rb_tree_iterator<value_type, const_reference, const_pointer>;
        using reverse_iterator          = toystl::reverse_iterator<iterator>;
        using const_reverse_iterator    = toystl::reverse_iterator<const_iterator>;

    protected:
        link_type get_node() { return nodeAllocator::allocate(1); }
        void put_node(link_type p) { nodeAllocator::deallocate(p);}

        link_type create_node(const value_type& x) {
            link_type tmp = get_node();
            try
            {
                construct(&tmp->value_field, x);
                
            }
            catch(...)
            {
                put_node(tmp);
                throw;
            }

            return tmp;
        }

        link_type clone_node(link_type x) {
            link_type tmp = create_node(x->value_field);
            tmp->color = x->color;
            tmp->left = x->left;
            tmp->right = x->right;
            
            return tmp;
        }

        void destroy_node(link_type p) {
            destroy(&p->value_field);
            put_node(p);
        }

    protected:
        // rb_tree 只以这三个成员变量
        size_type node_count;   // 树的大小（节点数量）
        link_type header;       // 实现上的一个技巧
        Compare key_compare;    // 节点间键值大小比较准则

        // 以下三个函数用来方便的取得 header 的成员
        link_type& root() const { return (link_type&)header->parent; }
        link_type& leftmost() const { return (link_type&)header->left; }
        link_type& rightmost() const { return (link_type&)header->right; }
        
        static link_type& left(link_type x)
        { return (link_type&)(x->left); }
        static link_type& right(link_type x)
        { return (link_type&)(x->right); }
        static link_type& parent(link_type x)
        { return (link_type&)(x->parent); }
        static reference value(link_type x)
        { return x->value_field; }
        static const Key& getKey(link_type x)
        { return KeyOfValue()(value(x)); }
        static rb_tree_color_type& color(link_type x) 
        { return (rb_tree_color_type&)(x->color); }

        static link_type& left(base_ptr x)
        { return (link_type&)(x->left); }
        static link_type& right(base_ptr x)
        { return (link_type&)(x->right); }
        static link_type& parent(base_ptr x)
        { return (link_type&)(x->parent); }
        static reference value(base_ptr x)
        { return ((link_type)x)->value_field; }
        static const Key& getKey(base_ptr x)
        { return KeyOfValue()(value(x)); }
        static rb_tree_color_type& color(base_ptr x) 
        { return (rb_tree_color_type&)(link_type(x)->color); }

        link_type minimum(link_type x) 
        { return (link_type) rb_tree_node_base::s_minimum(x); }
        link_type maximum(link_type x) 
        { return (link_type) rb_tree_node_base::s_maximum(x); }

    public:
        iterator insert(base_ptr x, base_ptr y, const value_type& v);
        link_type copy(link_type x, link_type y);
        void erase(link_type x);

        void clear();
        void init()
        {
            header = get_node();
            header->color = rb_tree_red; // header 为红色，用来区分 header 和 root
            root() = nullptr;            // header 的父节点为空
            leftmost() = header;         // header 的左子节点为自己
            rightmost() = header;        // header 的右子节点为自己
        }
    
    public:
        // 构造、赋值、析构函数
        rb_tree(const Compare& comp = Compare())
            : node_count(0),
              key_compare(comp)
        {
            init();
        }

        rb_tree(const rb_tree& rhs) {
            if(rhs.root() == nullptr) {
                init();
            }
            else {
                color(header) = rb_tree_red;
                root() = copy(rhs.root(), header);
                leftmost() = minimum(root());
                rightmost() = maximum(root());
            }
            node_count = rhs.node_count;
            key_compare = rhs.key_compare;
        }
        
        rb_tree& operator=(const rb_tree& rhs) {
            if(this != &rhs) {
                clear();

                if(rhs.root() == nullptr) {
                    root() == nullptr;
                    leftmost() = header;
                    rightmost() = header;
                }
                else {
                    root() = copy(rhs.root(), header);
                    leftmost() = minimum(root());
                    rightmost() = maximum(root());
                    node_count = rhs.node_count;
                }
            }

            return *this;
        }

        ~rb_tree()
        { clear(); }
    
    public:
        Compare key_comp() const { return key_compare; }
        iterator begin() { return (iterator)leftmost(); }
        const_iterator begin() const { return (iterator)leftmost(); }
        iterator end() { return header; }
        const_iterator end() const { return header; }

        reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rebegin() const { return const_reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        const_iterator cbegin() const { return begin(); }
        const_iterator cend() const { return end(); }
        const_reverse_iterator crbegin() const { return rbegin(); }
        const_reverse_iterator crend() const { return rend(); }

        // 容量相关操作
        bool empty() const { return node_count == 0; }
        size_type size() const { return node_count; }
        size_type max_size() const { return static_cast<size_type>(-1); }

        void swap(rb_tree& rhs) {
            if(this != &rhs) {
                toystl::swap(header, rhs.header);
                toystl::swap(node_count, rhs.node_count);
                toystl::swap(key_compare, rhs.key_compare);
            }
        }

        // 插入
        // 插入新值，节点键值允许重复
        iterator insert_equal(const value_type& v);
		void insert_equal(const_iterator first,const_iterator last);
		void insert_equal(const value_type* first,const value_type* last);
		template <class InputIterator>
		void insert_equal(InputIterator first,InputIterator last);
        // 插入新值，节点键值不允许重复，若重复则插入无效
		pair<iterator,bool> insert_unique(const value_type& v);
		void insert_unique(const_iterator first,const_iterator last);
		void insert_unique(const value_type* first,const value_type* last);
		template <class InputIterator>
		void insert_unique(InputIterator first,InputIterator last);

        // 删除
        void erase(iterator position);
        size_type erase(const key_type& x);
		void erase(iterator first, iterator last);
		void erase(const Key* first,const Key* last);

        // 查找
        iterator find(const key_type& k);
        const_iterator find(const key_type& k) const;
        size_type count(const key_type& x) const;
        
        //边界
        // 返回键值不小于 k 的第一个节点的迭代器
		iterator lower_bound(const Key& k);
		const_iterator lower_bound(const Key& k) const;
        // 返回键值不大于 k 的第一个节点的迭代器
		iterator upper_bound(const Key& k);
		const_iterator upper_bound(const Key& k) const;
		pair<iterator,iterator> equal_range(const Key& key);
		pair<const_iterator,const_iterator> equal_range(const Key& key) const; 
    };

    
    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
    void rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::clear()
    {
        if(node_count != 0) {
            erase(root());
            leftmost() = header;
            root() = 0;
            rightmost() = header;
            node_count = 0;
        }
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
    typename rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::iterator
    rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::insert_equal(const value_type& v)
    {
        link_type y = header;
        link_type x = root();  // 从根节点开始
        while(x != 0) {        // 从根节点开始，往下寻找适当的插入点
            y = x;
            x = key_compare(KeyOfValue()(v), getKey(x)) ? left(x) : right(x);
            // 以上，遇到大的则往左，遇到 小于等于 则往右
        }

        return insert(x, y, v);
        // 以上，x 为新值插入点，y 为插入点之父节点，v 为新值
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
    void rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        insert_equal(const_iterator first,const_iterator last)
    {
        for(; first != last; ++first) {
            insert_equal(*first);
        }
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
	void rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        insert_equal(const value_type* first,const value_type* last)
    {
        for(; first != last; ++first) {
            insert_equal(*first);
        }
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
	template <class InputIterator>
	void rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        insert_equal(InputIterator first,InputIterator last)
    {
        for(; first != last; ++first) {
            insert_equal(*first);
        }
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
    pair<typename rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::iterator,bool> 
    rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        insert_unique(const value_type& v)
    {
        link_type y = header;
        link_type x = root();
        bool comp = true;
        while(x != nullptr) {
            y = x;
            comp = key_compare(KeyOfValue()(v), getKey(x)); // v 的键值小于目前节点的键值？
            x = comp ? left(x) : right(x); // 遇大则往左，遇小于或等于则往右
        }
        // 离开 while 循环之后，y 所指就是插入点的父节点（此时的它必为叶子节点）
        
        iterator j = iterator(y);
        if(comp) { // 如果离开 while 循环时，comp 为真（表示遇 “大” ，将插入于左侧）
            if(j == begin()) { // 如果插入点的父节点为最左节点
                return pair<iterator, bool>(insert(x, y, v), true);
            }
            else {
                j--; // 调整 j,让 j 指向 比 j 处的节点键值小的那个节点的迭代器
            }
        }
        if(key_compare(getKey(j.node), KeyOfValue()(v))) // 判断这二者的键值是否相等。key_compare 为真，说明 getKey(j.node) > KeyOfValue()(v)
            // 新键值不与既有节点的简直重复，于是执行安插操作
            return pair<iterator, bool>(insert(x, y, v), true);
            // 以上 x 为新键值插入点，y 为插入点的父节点，v 为新值

        // 进行至此，表示新值一定与树中键值重复，那么就不应该插入新值
        return pair<iterator, bool>(j, false);
    }
    
    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
	void rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        insert_unique(const_iterator first,const_iterator last)
    {
        for(; first != last; ++first) {
            insert_unique(*first);
        }
    }
		
    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
    void rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        insert_unique(const value_type* first,const value_type* last)
    {
        for(; first != last; ++first) {
            insert_unique(*first);
        }
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
	template <class InputIterator>
	void rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        insert_unique(InputIterator first,InputIterator last)
    {
        for(; first != last; ++first) {
            insert_unique(*first);
        }
    }

    // 被其他函数调用，插入一定是插入到叶子节点
    // x 是新值插入点，y 是新值插入点父节点，参数 v 为新值
    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
    typename rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::iterator 
    rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        insert(base_ptr x, base_ptr y, const value_type& v)
    {
        link_type pos = static_cast<link_type>(x);  // 新值插入点
        link_type pos_parent = static_cast<link_type>(y);  // 新值插入点的父节点
        link_type z;                                       // 新值插入的节点

        // 如果新值插入点的父节点是 header 或者 新值插入点是空节点 或者 v 对应的键值 比 新插入点的父节点的键值要小
        if(pos_parent == header || pos != nullptr || key_compare(KeyOfValue()(v), getKey(pos_parent))) {
            z = create_node(v);
            left(pos_parent) = z;  // 将新的 value 的节点插入到 父节点的左边
            if(pos_parent == header) {
                root() = z;
                rightmost() = z;
            }
            else if(pos_parent == leftmost()) {
                leftmost() = z;
            }
        }
        else {
            z = create_node(v);
            right(pos_parent) = z;  // 将新的 value 的节点插入到 父节点的右边
            if(pos_parent == rightmost()) {
                rightmost() = z;
            }
        }

        parent(z) = pos_parent;
        left(z) = nullptr;
        right(z) = nullptr;
        rb_tree_rebalance(z, header->parent);
        ++node_count;
        return static_cast<iterator>(z);
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
    typename rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::link_type 
    rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        copy(link_type x, link_type y)
    {
        link_type top = clone_node(x);
        top->parent = y;

        try
        {
            if(x->right) {
                top->right = copy(right(x), top);
            }
            y = top;
            x = left(x);

            while(x != nullptr) {
                link_type p = clone_node(x);
                y->left = p;
                p->parent = y;
                if(x->right) {
                    p->right = copy(x->right, p);
                }
                y = p;
                x = x->left;
            }
        }
        catch(...)
        {
            erase(top);
            throw;
        }

        return top;
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
    void rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        erase(link_type x)
    {
        while(x != nullptr) {
            erase(right(x));
            link_type y = left(x);
            destroy_node(x);
            x = y;
        }
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
    void rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        erase(iterator position)
    {
        link_type y = static_cast<link_type>(rb_tree_rebalance_for_erase(position.node,
                                                                         header->parent,
                                                                         header->left,
                                                                         header->right));
        destroy_node(y);
        --node_count;
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
    typename rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::size_type 
    rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        erase(const key_type& x)
    {
        pair<iterator, iterator> p = equal_range(x);
        size_type n = distance(p.first, p.second);
        erase(p.first, p.second);

        return n;
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
    void rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        erase(iterator first, iterator last)
    {
        if(first == begin() && last = end()) {
            clear();
        } else {
            while(first != last) {
                erase(first++);
            }
        }
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
    void rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        erase(const Key* first,const Key* last)
    {
        while(first != last) {
            erase(*first++);
        }
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
    typename rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::iterator 
    rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        find(const key_type& k)
    {
        link_type y = header;
        link_type x = root();

        while(x != nullptr) {
            if(!key_compare(getKey(x), k)) {
                y = x;
                x = left(x);
            }
            else {
                x = right(x);
            }
        }

        iterator j = iterator(y);
        return (j == end() || key_compare(k, getKey(j.node))) ? end() : j;
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
    typename rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::const_iterator
    rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        find(const key_type& k) const
    {
        link_type y = header;
        link_type x = root();

        while(x != nullptr) {
            if(!key_compare(getKey(x), k)) {
                y = x;
                x = left(x);
            }
            else {
                x = right(x);
            }

            const_iterator j = const_iterator(y);
            return (j == end() || key_compare(k, getKey(j.node))) ? end() : j;
        }
    }
    
    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
    typename rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::size_type
    rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        count(const key_type& k) const
    {
        pair<const_iterator, const_iterator> p = equal_range(k);
        size_type n = 0;
        n = distance(p.first, p.second);

        return n;
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
    typename rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::iterator 
    rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        lower_bound(const Key& k)
    {
        link_type y = header;
        link_type x = root();

        while(x != nullptr) {
            if(!key_compare(getKey(x), k)) // x>=k
            {
                y = x;
                x = left(x);
            }
            else
            {
                x = right(x); // x < k
            }

            return iterator(y);
        }
    } 


    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
	typename rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::const_iterator 
    rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        lower_bound(const Key& k) const 
    {
        link_type y = header;
        link_type x = root();

        while(x != nullptr) {
            if(!key_compare(getKey(x), k)) // x>=k
            {
                y = x;
                x = left(x);
            }
            else
            {
                x = right(x);
            }

            return const_iterator(y);
        }
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
    typename rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::iterator 
    rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        upper_bound(const Key& k)
    {
        link_type y = header;
        link_type x = root();

        while(x != nullptr) {
            if(key_compare(k, getKey(x))) // k < x
            {
                y = x;
                x = left(x);
            }
            else
            {
                x = right(x);
            }

            return iterator(y);
        }
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
    typename rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::const_iterator 
    rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::
        upper_bound(const Key& k) const 
    {
        link_type y = header;
        link_type x = root();

        while(x != nullptr) {
            if(key_compare(k, getKey(x))) // x > k
            {
                y = x;
                x = left(x);
            }
            else
            {
                x = right(x);
            }

            return const_iterator(y);
        }
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
    pair<typename rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::iterator,
         typename rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::iterator> 
    rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::equal_range(const Key& key)
    {
        return pair<iterator, iterator>(lower_bound(key), upper_bound(key));
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
	pair<typename rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::const_iterator,
         typename rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::const_iterator>  
    rb_tree<Key,Value, KeyOfValue, Compare, Allocator>::equal_range(const Key& key) const
    {
        return pair<const_iterator, const_iterator>(lower_bound(key), upper_bound(key));
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
	inline bool 
	operator==(const rb_tree<Key, Value, KeyOfValue, Compare, Allocator>& x, 
	           const rb_tree<Key, Value, KeyOfValue, Compare, Allocator>& y)
	{
	  return x.size() == y.size() && toystl::equal(x.begin(), x.end(), y.begin());
	}

	template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
	inline bool 
	operator<(const rb_tree<Key, Value, KeyOfValue, Compare, Allocator>& x, 
	          const rb_tree<Key, Value, KeyOfValue, Compare, Allocator>& y)
	{
	  return toystl::lexicographical_compare(x.begin(), x.end(), 
	                                 y.begin(), y.end());
	}

	template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
	inline bool 
	operator!=(const rb_tree<Key, Value, KeyOfValue, Compare, Allocator>& x, 
	           const rb_tree<Key, Value, KeyOfValue, Compare, Allocator>& y)
    {
	  return !(x == y);
	}

	template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
	inline bool 
	operator>(const rb_tree<Key, Value, KeyOfValue, Compare, Allocator>& x, 
	          const rb_tree<Key, Value, KeyOfValue, Compare, Allocator>& y) {
	  return y < x;
	}

	template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
	inline bool 
	operator<=(const rb_tree<Key, Value, KeyOfValue, Compare, Allocator>& x, 
	           const rb_tree<Key, Value, KeyOfValue, Compare, Allocator>& y) {
	  return !(y < x);
	}

	template <class Key, class Value, class KeyOfValue, class Compare, class Allocator>
	inline bool 
	operator>=(const rb_tree<Key, Value, KeyOfValue, Compare, Allocator>& x, 
	           const rb_tree<Key, Value, KeyOfValue, Compare, Allocator>& y) {
	  return !(x < y);
	}

} // namespace toystl

#endif
