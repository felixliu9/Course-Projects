// Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

// felixliu

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

// A specialized version of the priority queue ADT implemented as a pairing
// heap.
template <typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR>
{
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node
    {
    public:
        // TODO: After you add add one extra pointer (see below), be sure
        //       to initialize it here.
        explicit Node(const TYPE &val)
            : elt{val}, child{nullptr}, sibling{nullptr}, previous{nullptr}
        {
        }

        // Description: Allows access to the element at that Node's
        //              position. There are two versions, getElt() and a
        //              dereference operator, use whichever one seems
        //              more natural to you.
        // Runtime: O(1) - this has been provided for you.
        const TYPE &getElt() const { return elt; }
        const TYPE &operator*() const { return elt; }

        // The following line allows you to access any private data
        // members of this Node class from within the PairingPQ class.
        // (ie: myNode.elt is a legal statement in PairingPQ's add_node()
        // function).
        friend PairingPQ;

    private:
        TYPE elt;
        Node *child;
        Node *sibling;
        Node *previous;
    }; // Node

    // Description: Construct an empty pairing heap with an optional
    //              comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) : BaseClass{comp}
    {
        count = 0;
        root = nullptr;
    } // PairingPQ()

    // Description: Construct a pairing heap out of an iterator range with an
    //              optional comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template <typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end,
              COMP_FUNCTOR comp = COMP_FUNCTOR()) : 
              BaseClass{comp}
    {
        for (InputIterator it = start; it != end; ++it)
        {
            push(*it);
        }
    } // PairingPQ()

    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other) : BaseClass{other.compare}, count(other.count)
    {
        root = copy(other.root);
    } // PairingPQ()

    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs)
    {
        // delete current nodes
        this->clear();
        this->count = rhs.count;
        this->compare = rhs.compare;
        this->root = copy(rhs.root);

        return *this;
    } // operator=()

    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ()
    {
        this->clear();
    } // ~PairingPQ()

    // Description: Assumes that all elements inside the pairing heap are out
    //              of order and 'rebuilds' the pairing heap by fixing the
    //              pairing heap invariant. You CANNOT delete 'old' nodes
    //              and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities()
    {
        if (root == nullptr || root->child == nullptr)
        {
            return;
        }

        std::deque<Node *> nodes;
        Node *child = root->child;
        child->previous = nullptr;
        root->child = nullptr;
        nodes.push_back(child);

        while (!nodes.empty())
        {
            Node *node = nodes.front();
            nodes.pop_front();
            if (node->child)
            {
                nodes.push_back(node->child);
                node->child->previous = nullptr;
                node->child = nullptr;
            }
            if (node->sibling)
            {
                nodes.push_back(node->sibling);
                node->sibling->previous = nullptr;
                node->sibling = nullptr;
            }
            root = meld(root, node);
        }

    } // updatePriorities()

    // Description: Add a new element to the pairing heap. This is already
    //              done. You should implement push functionality entirely in
    //              the addNode() function, and this function calls
    //              addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val)
    {
        addNode(val);
    } // push()

    // Description: Remove the most extreme (defined by 'compare') element
    //              from the pairing heap.
    // Note: We will not run tests on your code that would require it to pop
    //       an element when the pairing heap is empty. Though you are
    //       welcome to if you are familiar with them, you do not need to use
    //       exceptions in this project.
    // Runtime: Amortized O(log(n))
    virtual void pop()
    {
        if (root == nullptr)
        {
            return;
        }
        std::deque<Node *> nodes;
        Node *child = root->child;

        while (child)
        {
            child->previous = nullptr;
            Node *tmp = child->sibling;
            child->sibling = nullptr;
            nodes.push_back(child);
            child = tmp;
        }

        delete root;
        root = nullptr;
        if (nodes.size() > 0)
        {
            root = meld_many(nodes);
        }
        count--;
    } // pop()

    // Description: Return the most extreme (defined by 'compare') element of
    //              the pairing heap. This should be a reference for speed.
    //              It MUST be const because we cannot allow it to be
    //              modified, as that might make it no longer be the most
    //              extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const
    {
        return root->elt;
    } // top()

    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    virtual std::size_t size() const
    {
        return count;
    } // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    virtual bool empty() const
    {
        return count == 0;
    }
    // Description: Updates the priority of an element already in the pairing
    //              heap by replacing the element refered to by the Node with
    //              new_value. Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more
    //               extreme (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node *node, const TYPE &new_value)
    {
        node->elt = new_value;
        // detach updated node
        std::deque<Node *> nodes;
        nodes.push_back(node);
        if (node->sibling)
        {
            if (node->previous)
            {
                if (node == node->previous->child)
                {
                    // previous is parent
                    node->previous->child = node->sibling;
                    node->sibling->previous = node->previous;
                }
                else
                {
                    // previous is sibling
                    node->previous->sibling = node->sibling;
                    node->sibling->previous = node->previous;
                }

                node->previous = nullptr;
                node->sibling = nullptr;
                nodes.push_back(root);
            }
            else
            {
                // a Node with sibling must have a previous, either
                // its sibling or parent
                throw std::invalid_argument("A Node with sibling must have a previous.");
            }
        }
        else
        {
            if (node->previous)
            {
                if (node == node->previous->child)
                {
                    node->previous->child = nullptr;
                }
                else
                {
                    node->previous->sibling = nullptr;
                }
                node->previous = nullptr;
                node->sibling = nullptr;
                nodes.push_back(root);
            }
            else
            {
                // root node
                Node *child = node->child;
                while (child)
                {
                    child->previous = nullptr;
                    Node *next = child->sibling;
                    child->sibling = nullptr;
                    nodes.push_back(child);
                    child = next;
                }
            }
        }
        root = meld_many(nodes);

    } // updateElt()

    // Description: Add a new element to the pairing heap. Returns a Node*
    //              corresponding to the newly added element.
    // NOTE: Whenever you create a node, and thus return a Node *, you must
    //       be sure to never move or copy/delete that node in the future,
    //       until it is eliminated by the user calling pop(). Remember this
    //       when you implement updateElt() and updatePriorities().
    // Runtime: O(1)
    Node *addNode(const TYPE &val)
    {
        count++;
        Node *node = new Node(val);
        if (root)
        {
            root = meld(node, root);
        }
        else
        {
            root = node;
        }

        return node;
    } // addNode()

private:
    uint32_t count = 0;
    Node *root = nullptr;

    Node *meld(Node *node1, Node *node2)
    {
        if (this->compare(node1->elt, node2->elt))
        {
            Node *cur_child = node2->child;
            node2->child = node1;
            node1->previous = node2;
            if (cur_child)
            {
                node1->sibling = cur_child;
                cur_child->previous = node1;
            }
            return node2;
        }else{
            Node *cur_child = node1->child;
            node1->child = node2;
            node2->previous = node1;
            if (cur_child)
            {
                node2->sibling = cur_child;
                cur_child->previous = node2;
            }
            return node1;
        }
    }

    Node *meld_many(std::deque<Node *> &nodes)
    {
        while (nodes.size() > 1)
        {
            Node *node1 = nodes.front();
            nodes.pop_front();
            Node *node2 = nodes.front();
            nodes.pop_front();

            nodes.push_back(meld(node1, node2));
        }

        return nodes.front();
    }

    Node *copy(Node *other)
    {
        if (other == nullptr)
        {
            return nullptr;
        }

        Node *newRoot = nullptr;

        std::deque<Node *> nodes;
        nodes.push_back(other);

        while (!nodes.empty())
        {
            Node *node = nodes.front();
            nodes.pop_front();
            if (node->child)
            {
                nodes.push_back(node->child);
            }
            if (node->sibling)
            {
                nodes.push_back(node->sibling);
            }

            Node *newNode = new Node(node->elt);
            if (newRoot)
            {
                newRoot = meld(newRoot, newNode);
            }
            else
            {
                newRoot = newNode;
            }
        }
        return newRoot;
    }

    void clear()
    {
        if (root == nullptr)
        {
            return;
        }

        std::deque<Node *> nodes;
        nodes.push_back(root);

        while (!nodes.empty())
        {
            Node *node = nodes.front();
            nodes.pop_front();
            if (node->child)
            {
                nodes.push_back(node->child);
            }
            if (node->sibling)
            {
                nodes.push_back(node->sibling);
            }
            delete node;
        }
        count = 0;
        root = nullptr;
    }
};

#endif // PAIRINGPQ_H
