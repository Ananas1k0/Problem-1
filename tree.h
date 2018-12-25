#pragma once

#include <memory>

namespace bintree {
	template <typename T>

	//enable_shared_from_this allows an object TNode (this) to safely generate additional 
	//std::shared_ptr instances in replaceLeft and replaceRight that all share ownership of TNode (this). 
	struct TNode : std::enable_shared_from_this< TNode< T > > {
		using TNodePtr = std::shared_ptr<TNode<T>>;
		using TNodeConstPtr = std::shared_ptr<const TNode<T>>;

		bool hasLeft() const {
			return bool(left);
		}

		bool hasRight() const {
			return bool(right);
		}

		bool hasParent() const {
			return bool(parent);
		}

		T& getValue() {
			return value;
		}

		const T& getValue() const {
			return value;
		}

		TNodePtr getLeft() {
			return left;
		}

		TNodeConstPtr getLeft() const {
			return left;
		}

		TNodePtr getRight() {
			return right;
		}

		TNodeConstPtr getRight() const {
			return right;
		}

		TNodePtr getParent() {
			//we should use here lock that creates a shared_ptr that manages the referenced object
			//If there is no managed object, i.e. *this is empty, then the returned shared_ptr also is empty.
			return parent.lock();
		}

		TNodeConstPtr getParent() const {
			//we should use here lock that creates a shared_ptr that manages the referenced object
			//If there is no managed object, i.e. *this is empty, then the returned shared_ptr also is empty.
			return parent.lock();
		}

		static TNodePtr createLeaf(T v) {
			//Here we will return the successor of TNode to use it constructor.
			//It's required because we can't change the interface of TNode but all constructors are private.
			return std::make_shared<TSuccessorNode>(v);
		}

		static TNodePtr fork(T v, TNodePtr left, TNodePtr right) {
			//Here we will return the successor of TNode to use it constructor.
            //It's required because we can't change the interface of TNode but all constructors are private.
			TNodePtr ptr = std::make_shared<TSuccessorNode>(v, left, right);
			setParent(ptr->getLeft(), ptr);
			setParent(ptr->getRight(), ptr);
			return ptr;
		}

		TNodePtr replaceLeft(TNodePtr l) {
			//shared_from_this method used here to avoid the creatition of the new TNodePtr object.
			//Instead of it we will use the existed one (this).
			//So std::shared_ptr instances in replaceLeft and replaceRight that all share ownership of TNode (this).
			setParent(l, this->shared_from_this());
			setParent(left, nullptr);
			std::swap(l, left);
			return l;
		}

		TNodePtr replaceRight(TNodePtr r) {
			//shared_from_this method used here to avoid the creatition of the new TNodePtr object.
			//Instead of it we will use the existed one (this).
			//So std::shared_ptr instances in replaceLeft and replaceRight that all share ownership of TNode (this).
			setParent(r, this->shared_from_this());
			setParent(right, nullptr);
			std::swap(r, right);
			return r;
		}

		TNodePtr replaceRightWithLeaf(T v) {
			return replaceRight(createLeaf(v));
		}

		TNodePtr replaceLeftWithLeaf(T v) {
			return replaceLeft(createLeaf(v));
		}

		TNodePtr removeLeft() {
			return replaceLeft(nullptr);
		}
		TNodePtr removeRight() {
			return replaceRight(nullptr);
		}

	private:
		T value;
		TNodePtr left = nullptr;
		TNodePtr right = nullptr;

		//weak reference is required here to be sure that the memory will be deallocated in case of 
		//circular references. For example in case if two shared_ptr for TNode object will have 
		//shared pointers pointing to each other (T1 with parent T2 and T2 with parent T1)
		//std::weak_ptr is used to track the TNode object, and it is converted to std::shared_ptr to assume temporary ownership. 
		//If the original std::shared_ptr is destroyed at this time, the object's lifetime is extended 
		//until the temporary std::shared_ptr is destroyed as well.
		std::weak_ptr< TNode< T > > parent;

		TNode(T v)
			: value(v)
		{
		}

		//shared_ptr (TNodePtr) is used here instead of raw_ptr (TNode*) because it give the guaranty that
		//the object will be destroyed and its memory be deallocated when either of the following happens :
		// 1)the last remaining shared_ptr owning the object is destroyed;
		// 2)the last remaining shared_ptr owning the object is assigned another pointer via operator= or reset().
		TNode(T v, TNodePtr left, TNodePtr right)
			: value(v)
			, left(left)
			, right(right)
		{
		}

		//because all the constructors are private we will create a private successor of TNode to use its ones
		//in createLeaf and fork methods.
		struct TSuccessorNode : public TNode {
			TSuccessorNode(T v) : TNode(v) {}
			TSuccessorNode(T v, TNodePtr left, TNodePtr right) : TNode(v, left, right) {}
		};

		static void setParent(TNodePtr node, TNodePtr parent) {
			if (node)
				node->parent = parent;
		}
	};
}