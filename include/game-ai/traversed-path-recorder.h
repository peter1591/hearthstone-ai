#pragma once

#include <list>

class TreeNode;

class TraversedPathRecorderReverseIterator
{
public:
	typedef std::list<TreeNode*> list_type;

public:
	TraversedPathRecorderReverseIterator(TreeNode const* leaf, list_type const& preceeding_nodes);

	TreeNode const* GetNodeAndMoveUpward();

private:
	void FillNextRedirectInfo();
	TreeNode const* GetParentNodeWithoutRedirectAndMoveUpward();

private:
	list_type const& preceeding_nodes;

	TreeNode const* current_node;
	list_type::const_reverse_iterator it_preceeding_nodes;

	TreeNode const* next_redirect_source;
	TreeNode const* next_redirect_target;
	TreeNode const* next_redirect_parent;
};

class TraversedPathRecorder
{
public:
	void Clear();

	void AddRedirectNode(TreeNode * redirect_node);

	void AddHiddenRedirectNode(TreeNode * redirect_parent, TreeNode * redirect_target);

	TraversedPathRecorderReverseIterator GetReverseIterator(TreeNode const* leaf) const;

private:
	typedef TraversedPathRecorderReverseIterator::list_type list_type;
	list_type preceeding_nodes;
};