#include <bits/stdc++.h>
using namespace std;

/// A class to create a node for the B+ tree with order `m`
/// Each internal node will have a minimum of `ceil(m/2)` keys and a maximum of `(m-1)` keys
/// Each leaf node will have a minimum of `ceil((m-1)/2)` keys and a maximum of `(m-1)` keys
class Node {

    public:
        int m;
        vector<int> keys;
        vector<Node*> pointers;
        bool isLeaf;

        Node(int order, bool leaf) {
            m = order;
            isLeaf = leaf;
            pointers.resize(m, NULL);
        }

        void insertKey(int key);
        bool isEmpty();
        bool isFull();
        void display();
};

/// A class to create a right-biased B+ Tree
class BPlusTree {

    public:
        int m;
        Node* root;

        BPlusTree(int order) {
            m = order;
            root = new Node(m, true);
        }

        pair<Node*,Node*> search(int key);
        void insert(int key);
        void insertIntoInternalNode(Node* parent, Node* child, int key);
        Node* getParentNode(Node* node);
        void deleteKey(int key);
        void mergeInternal(Node* node);
        void deleteFromInternal(int key);
        void display();
};

/// Utility function to set all the values of `vec` to `NULL`
void setNull(vector<Node*> &vec) {
    for (auto &val : vec) val = NULL;
}

/// Node functions

/// Function to check if the node is full
bool Node::isFull() {
    return keys.size() == m-1;
}

/// Function to check if the node is empty
bool Node::isEmpty() {
    return keys.size() == 0;
}

/// Function to insert a `key` in the node
void Node::insertKey(int key) {
    keys.push_back(key);
    sort(keys.begin(), keys.end());
}

/// Function to display the node
void Node::display() {
    cout << "[ ";
    for (auto key : keys) {
        cout << key << " ";
    }
    cout << "]";
}

/// BPlusTree functions

/// Function to insert a `key` in the B+ tree
void BPlusTree::insert(int key) {

    if (search(key).second != NULL) {
        cout << "\nKey already exists!\n\n";
        return;
    }

    if (root->isEmpty()) {
        root->insertKey(key);
        return;
    }
    else {
        /// handle the root case differently
        if (root->isLeaf) {
            if (!root->isFull()) {
                root->insertKey(key);
            } else {
                Node* newRoot = new Node(m, false);
                Node* newLeaf = new Node(m, true);
                Node* currentLeaf = root;

                vector<int> keyValues = currentLeaf->keys;
                keyValues.push_back(key);
                sort(keyValues.begin(), keyValues.end());
                currentLeaf->keys.clear();
                currentLeaf->isLeaf = true;

                int left = (int)ceil((m-1)/2.0);
                int right = m-left;
                for (int i = 0; i < left; i++) {
                    currentLeaf->insertKey(keyValues[0]);
                    keyValues.erase(keyValues.begin());
                }
                int parentKey = keyValues[0];
                for (int i = 0; i < right; i++) {
                    newLeaf->insertKey(keyValues[0]);
                    keyValues.erase(keyValues.begin());
                }

                currentLeaf->pointers.back() = newLeaf;
                newRoot->pointers[0] = currentLeaf;
                newRoot->pointers[1] = newLeaf;
                newRoot->insertKey(parentKey);

                this->root = newRoot;
            }
        } else {
            /// move to the leaf node where the `key` needs to be inserted
            Node* currentLeaf = root;
            Node* parent;
            while (!currentLeaf->isLeaf) {
                parent = currentLeaf;
                for (int i = 0; i < currentLeaf->keys.size(); i++) {
                    if (key < currentLeaf->keys[i]) {
                        currentLeaf = currentLeaf->pointers[i];
                        goto nxt;
                    }
                }
                if (key >= currentLeaf->keys.back())
                    currentLeaf = currentLeaf->pointers[currentLeaf->keys.size()];
                nxt:{}
            }

            /// simply insert the key in the node if it has space
            if (!currentLeaf->isFull()) {
                currentLeaf->insertKey(key);
            } else {
                /// split the leaf into two if it already contains the maximum number of keys
                Node* newLeaf = new Node(m, true);
                vector<int> keyValues = currentLeaf->keys;
                keyValues.push_back(key);
                sort(keyValues.begin(), keyValues.end());
                currentLeaf->keys.clear();

                int left = (int)ceil((m-1)/2.0);
                int right = m-left;
                for (int i = 0; i < left; i++) {
                    currentLeaf->insertKey(keyValues[0]);
                    keyValues.erase(keyValues.begin());
                }
                int parentKey = keyValues[0];
                for (int i = 0; i < right; i++) {
                    newLeaf->insertKey(keyValues[0]);
                    keyValues.erase(keyValues.begin());
                }

                currentLeaf->pointers.back() = newLeaf;

                /// insert key into internal node 
                insertIntoInternalNode(parent, newLeaf, parentKey);
            }
        }
    }
}

/// Function to insert a key in an internal node of the B+ tree
void BPlusTree::insertIntoInternalNode(Node* parent, Node* child, int key) {
    /// simply insert the key if the node is not full and rearrange pointers
    if (!parent->isFull()) {
        int pos = 0;
        while (key > parent->keys[pos] && pos < parent->keys.size())
            pos++;
        parent->keys.push_back(0);
        for (int i = parent->keys.size()-1; i > pos; i--) {
            parent->keys[i] = parent->keys[i-1];
        }
        for (int i = parent->keys.size(); i > pos+1; i--) {
            parent->pointers[i] = parent->pointers[i-1];
        }
        parent->keys[pos] = key;
        parent->pointers[pos+1] = child;

    } else {
        /// split the internal node into two if it is already full
        vector<Node*> pointers = parent->pointers;
        pointers.push_back(child);
        vector<int> keyValues = parent->keys;
        keyValues.push_back(key);
        sort(keyValues.begin(), keyValues.end());
        setNull(parent->pointers);
        parent->keys.clear();

        Node* newInternal = new Node(m, false);

        /// rearrange keys
        int left = (int)ceil(m/2.0)-1;
        int right = m-1-left;
        for (int i = 0; i < left; i++) {
            parent->keys.push_back(keyValues[0]);
            keyValues.erase(keyValues.begin());
        }
        int parentKey = keyValues[0];
        keyValues.erase(keyValues.begin());
        for (int i = 0; i < right; i++) {
            newInternal->keys.push_back(keyValues[0]);
            keyValues.erase(keyValues.begin());
        }

        /// rearrange pointers
        int leftPtrs = (int)ceil(m/2.0);
        int rightPtrs = m+1-leftPtrs;
        for (int i = 0; i < leftPtrs; i++) {
            parent->pointers[i] = pointers[0];
            pointers.erase(pointers.begin());
        }
        for (int i = 0; i < rightPtrs; i++) {
            newInternal->pointers[i] = pointers[0];
            pointers.erase(pointers.begin());
        }

        /// the base condition for the recursive splitting of internal nodes
        if (parent == root) {
            Node* newRoot = new Node(m, false);
            newRoot->insertKey(parentKey);
            newRoot->pointers[0] = parent;
            newRoot->pointers[1] = newInternal;
            this->root = newRoot;
        }

        else {
            /// call the function recursively
            insertIntoInternalNode(getParentNode(parent), newInternal, parentKey);
        }

    }
}

/// Function to get the parent node of a particular `node`
Node* BPlusTree::getParentNode(Node *node) {

    /// BFS traversal to find the parent of a particular node
    queue<Node*> q;
    q.push(root);
    while (!q.empty()) {

        int l = q.size();
        for (int i = 0; i < l; i++) {
            Node* curr = q.front();
            q.pop();
 
            for (int j = 0; j <= curr->keys.size(); j++)
                if (curr->pointers[j] != NULL && curr->pointers[j] == node)
                    return curr;
        }
    }
    return NULL;
}

/// Function to search a `key` in the leaf nodes and return the parent and the node in which the key is found
pair<Node*,Node*> BPlusTree::search(int key) {
    Node* currentLeaf = root;
    Node* parent = NULL;
    while (!currentLeaf->isLeaf) {
        parent = currentLeaf;
        for (int i = 0; i < currentLeaf->keys.size(); i++) {
            if (key < currentLeaf->keys[i]) {
                currentLeaf = currentLeaf->pointers[i];
                goto nxt;
            }
        }
        if (key >= currentLeaf->keys.back())
            currentLeaf = currentLeaf->pointers[currentLeaf->keys.size()];
        nxt:{}
    }
    for (int i = 0; i < currentLeaf->keys.size(); i++) {
        if (key == currentLeaf->keys[i])
            return {parent,currentLeaf};
    }
    return {NULL,NULL};
}

/// Function to delete a `key` from the B+ tree
void BPlusTree::deleteKey(int key) {

    pair<Node*,Node*> pr = search(key);
    Node* parent = pr.first;
    Node* currentLeaf = pr.second;

    /// check if the key is present in the B+ tree
    if (parent == NULL && currentLeaf == NULL) {
        cout << "Key not present!\n\n";
        return;
    }

    /// handle the root case differently
    if (parent == NULL) {
        root->keys.erase(find(root->keys.begin(), root->keys.end(), key));
        sort(root->keys.begin(), root->keys.end());
        return;
    }

    int minimum = (int)ceil((m-1)/2.0);
    if (currentLeaf->keys.size() > minimum) {
        currentLeaf->keys.erase(find(currentLeaf->keys.begin(), currentLeaf->keys.end(), key));
        sort(currentLeaf->keys.begin(), currentLeaf->keys.end());
        deleteFromInternal(key);
        return;
    }

    /// get the left and right sibling indices
    int left = -1, right = -1;
    for (int i = 0; i < parent->keys.size(); i++) {
        if (key < parent->keys[i]) {
            left = i-1, right = i+1;
            break;
        }
    }
    
    if (key >= parent->keys.back()) {
        left = (int)(parent->keys.size())-1;
        right = left + 2;
    }

    /// borrow a key from the left sibling if possible
    if (left >= 0 && parent->pointers[left]->keys.size() > minimum) {
        
        currentLeaf->keys.erase(find(currentLeaf->keys.begin(), currentLeaf->keys.end(), key));
        sort(currentLeaf->keys.begin(), currentLeaf->keys.end());

        Node* leftSibling = parent->pointers[left];
        int borrowKey = leftSibling->keys.back();
        leftSibling->keys.pop_back();
        currentLeaf->keys.insert(currentLeaf->keys.begin(), borrowKey);
        parent->keys[left] = borrowKey;
    }

    /// borrow a key from the right sibling if possible
    else if (right <= parent->keys.size() && parent->pointers[right]->keys.size() > minimum) {
        currentLeaf->keys.erase(find(currentLeaf->keys.begin(), currentLeaf->keys.end(), key));
        sort(currentLeaf->keys.begin(), currentLeaf->keys.end());

        Node* rightSibling = parent->pointers[right];
        int borrowKey = rightSibling->keys[0];
        rightSibling->keys.erase(rightSibling->keys.begin());
        currentLeaf->keys.push_back(borrowKey);
        parent->keys[right-1] = rightSibling->keys[0];
    }

    /// merge with left sibling if it exists
    else if (left >= 0) {
        currentLeaf->keys.erase(find(currentLeaf->keys.begin(), currentLeaf->keys.end(), key));
        sort(currentLeaf->keys.begin(), currentLeaf->keys.end());

        Node* leftSibling = parent->pointers[left];
        vector<int> keys;
        for (auto k : leftSibling->keys) {
            keys.push_back(k);
        }
        for (auto k : currentLeaf->keys) {
            keys.push_back(k);
        }
        leftSibling->keys.clear(), currentLeaf->keys.clear();

        Node* newLeaf = new Node(m, true);
        for (auto k : keys) {
            newLeaf->keys.push_back(k);
            sort(newLeaf->keys.begin(), newLeaf->keys.end());
        }

        
        parent->pointers[left] = newLeaf;
        for (int i = left+1; i < parent->keys.size(); i++) {
            parent->keys[i-1] = parent->keys[i];
            parent->pointers[i] = parent->pointers[i+1];
        }
        parent->pointers[parent->keys.size()] = NULL;
        parent->keys.pop_back();

        if (left-1 >= 0) {
            parent->keys[left-1] = parent->pointers[left]->keys[0];
        }

        mergeInternal(parent);

    }

    /// merge with right sibling if no other case is possible
    else {
        currentLeaf->keys.erase(find(currentLeaf->keys.begin(), currentLeaf->keys.end(), key));
        sort(currentLeaf->keys.begin(), currentLeaf->keys.end());

        Node* rightSibling = parent->pointers[right];
        vector<int> keys;
        for (auto k : rightSibling->keys) {
            keys.push_back(k);
        }
        for (auto k : currentLeaf->keys) {
            keys.push_back(k);
        }
        rightSibling->keys.clear(), currentLeaf->keys.clear();

        Node* newLeaf = new Node(m, true);
        for (auto k : keys) {
            newLeaf->keys.push_back(k);
            sort(newLeaf->keys.begin(), newLeaf->keys.end());
        }

        parent->pointers[right-1] = newLeaf;
        for (int i = right; i < parent->keys.size(); i++) {
            parent->keys[i-1] = parent->keys[i];
            parent->pointers[i] = parent->pointers[i+1];
        }
        parent->pointers[parent->keys.size()] = NULL;
        parent->keys.pop_back();

        if (right-2 >= 0) {
            parent->keys[right-2] = parent->pointers[right-1]->keys[0];
        }

        mergeInternal(parent);
    }
    /// delete/update the internal nodes to remove the `key` if it is still present
    deleteFromInternal(key);
}

/// Function to merge an internal `node` if underflow occurs
void BPlusTree::mergeInternal(Node* node) {

    int minimum = (int)ceil(m/2.0);
    if (node->keys.size() >= minimum)
        return;

    /// base condition for the recursive merging of internal nodes
    if (node == root) {
        if (!root->isEmpty()) 
            return;
        root = root->pointers[0];
        return;
    }

    /// get the left and right sibling indices
    Node* parent = getParentNode(node);
    int left = -1, right = -1;
    for (int i = 0; i <= parent->keys.size(); i++) {
        if (node == parent->pointers[i]) {
            left = i-1, right = i+1;
            break;
        }
    }

    /// get a key from the left sibling if possible
    if (left >= 0 && parent->pointers[left]->keys.size() > minimum) {

        Node* leftSibling = parent->pointers[left];
        Node* ptr = leftSibling->pointers[leftSibling->keys.size()];
        int leftKey = leftSibling->keys.back();
        int parentKey = parent->keys[left];
        leftSibling->pointers[leftSibling->keys.size()] = NULL;
        leftSibling->keys.pop_back();

        node->keys.insert(node->keys.begin(), parentKey);
        rotate(node->pointers.begin(), node->pointers.begin() + node->pointers.size() - 1, node->pointers.end());
        node->pointers[0] = ptr;
        parent->keys[left] = leftKey;
    }
    /// get a key from the right sibling if possible
    else if (right <= parent->keys.size() && parent->pointers[right]->keys.size() > minimum) {

        Node* rightSibling = parent->pointers[right];
        Node* ptr = rightSibling->pointers[0];
        int rightKey = rightSibling->keys[0];
        int parentKey = parent->keys[right-1];
        rightSibling->pointers[0] = NULL;
        rightSibling->keys.erase(rightSibling->keys.begin());
        rotate(rightSibling->pointers.begin(), rightSibling->pointers.begin() + 1, rightSibling->pointers.end());

        node->keys.push_back(parentKey);
        node->pointers[node->keys.size()] = ptr;
        parent->keys[right-1] = rightKey;
    }
    /// merge with left sibling if it exists
    else if (left >= 0) {
        Node* newInternal = new Node(m, false);
        int parentKey = parent->keys[left];
        Node* leftSibling = parent->pointers[left];
        for (auto k : leftSibling->keys) {
            newInternal->keys.push_back(k);
        }
        newInternal->keys.push_back(parentKey);
        for (auto k : node->keys) {
            newInternal->keys.push_back(k);
        }
        for (int i = 0; i <= leftSibling->keys.size(); i++) {
            newInternal->pointers[i] = leftSibling->pointers[i];
        }
        for (int i = 0; i <= node->keys.size(); i++) {
            newInternal->pointers[i+leftSibling->keys.size()+1] = node->pointers[i];
        }

        parent->pointers[left] = newInternal;
        for (int i = left+1; i < parent->keys.size(); i++) {
            parent->keys[i-1] = parent->keys[i];
            parent->pointers[i] = parent->pointers[i+1];
        }
        parent->pointers[parent->keys.size()] = NULL;
        parent->keys.pop_back();

        mergeInternal(parent);
    }
    /// merge with right sibling if no other case is possible
    else {
        Node* newInternal = new Node(m, false);
        int parentKey = parent->keys[right-1];
        Node* rightSibling = parent->pointers[right];
        for (auto k : node->keys) {
            newInternal->keys.push_back(k);
        }
        newInternal->keys.push_back(parentKey);
        for (auto k : rightSibling->keys) {
            newInternal->keys.push_back(k);
        }
        for (int i = 0; i <= node->keys.size(); i++) {
            newInternal->pointers[i] = node->pointers[i];
        }
        for (int i = 0; i <= rightSibling->keys.size(); i++) {
            newInternal->pointers[i+node->keys.size()+1] = rightSibling->pointers[i];
        }

        parent->pointers[right-1] = newInternal;
        for (int i = right; i < parent->keys.size(); i++) {
            parent->keys[i-1] = parent->keys[i];
            parent->pointers[i] = parent->pointers[i+1];
        }
        parent->pointers[parent->keys.size()] = NULL;
        parent->keys.pop_back();

        mergeInternal(parent);
    }
    
} 

/// Function to delete `key` from internal nodes
void BPlusTree::deleteFromInternal(int key) {

    Node* node = NULL;
    queue<Node*> q;
    q.push(root);
    int idx = -1;
    while (!q.empty()) {
        Node* curr = q.front();
        q.pop();
        for (int i = 0; i < curr->keys.size(); i++) {
            if (curr->keys[i] == key) {
                node = curr;
                idx = i;
                goto end;
            }
        }
        for (auto k : curr->pointers)
            if (k != NULL) q.push(k);
    }
    end:{}
    if (node) {
        Node* curr = node->pointers[idx+1];
        while (!curr->isLeaf) {
            curr = curr->pointers[0];
        }
        node->keys[idx] = curr->keys[0];
    }
}

/// Function to display a BFS traversal of the B+ tree
void BPlusTree::display() {

    queue<Node*> q;
    q.push(root);

    while (!q.empty()) {
        int l = q.size();
        for (int i = 0; i < l; i++) {
            Node* curr = q.front();
            q.pop();
            curr->display();
 
            for (int j = 0; j <= curr->keys.size(); j++)
                if (curr->pointers[j] != NULL && !curr->isLeaf)
                    q.push(curr->pointers[j]);
            cout << "    ";
        }
        cout << endl;
    }
}

signed main() {

    cout << "Enter order of the B+ tree: ";
    int order;
    cin >> order;
    BPlusTree bp = BPlusTree(order);

    for(;;) {
        cout << "\nChoose an operation\n";
        cout << "1. Insert a key" << endl
             << "2. Delete a key" << endl
             << "3. Display the B+ tree" << endl
             << "4. Exit the program" << endl;

        int option;
        cin >> option;
        switch (option) {

            case 1:
                cout << "Enter the key you want to insert: ";
                int key;
                cin >> key;
                bp.insert(key);
                break;

            case 2:
                cout << "Enter the key you want to delete: ";
                cin >> key;
                bp.deleteKey(key);
                break;
            
            case 3:
                cout << endl;
                bp.display();
                break;

            case 4:
                goto end;
                break;
            
            default:
                cout << "Please choose a valid option\n\n";
                break;
        }
    }
    end: {}
    return 0;
}