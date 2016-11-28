#pragma once

#include "traversed-path-recorder.h"
#include "tree.h"

inline TraversedPathRecorderReverseIterator::TraversedPathRecorderReverseIterator(TreeNode const* root_node, TreeNode const* leaf, list_type const& preceeding_nodes) :
	root_node(root_node), preceeding_nodes(preceeding_nodes)
{
	this->current_node = leaf;
	this->it_preceeding_nodes = this->preceeding_nodes.crbegin();
	this->FillNextRedirectInfo();
}

inline TreeNode const * TraversedPathRecorderReverseIterator::GetNodeAndMoveUpward()
{
	if (this->current_node == nullptr) return nullptr;

	if (this->current_node == this->next_redirect_target) {
		// follow redirect info

		if (this->next_redirect_source) {
			// have redirect node
			TreeNode const* ret = this->next_redirect_source;
			this->current_node = this->next_redirect_parent;
			this->FillNextRedirectInfo();
			return ret;
		}
		else {
			// the redirect node is hidden (not created)
			TreeNode const* ret = this->next_redirect_target; // the 'move' of redirect target and source are ensured to be the same
			this->current_node = this->next_redirect_parent;
			this->FillNextRedirectInfo();
			return ret;
		}
	}
	else {
		if (this->current_node == this->root_node) {
			this->current_node = nullptr;
			return nullptr;
		}

		TreeNode const* ret = this->current_node;
		this->current_node = this->current_node->parent;
		return ret;
	}
}

inline void TraversedPathRecorderReverseIterator::FillNextRedirectInfo()
{
	if (this->it_preceeding_nodes == this->preceeding_nodes.crend())
	{
		this->next_redirect_target = nullptr;
	}
	else {
		auto const& next_record_node = *this->it_preceeding_nodes;

		if (next_record_node->equivalent_node) {
			this->next_redirect_source = next_record_node;
			this->next_redirect_target = next_record_node->equivalent_node;
			this->next_redirect_parent = next_record_node->parent;
			++this->it_preceeding_nodes;
		}
		else {
			this->next_redirect_source = nullptr; // the redirect node is optimized out (not created)
			this->next_redirect_target = *this->it_preceeding_nodes;
			++this->it_preceeding_nodes;
			this->next_redirect_parent = *this->it_preceeding_nodes;
			++this->it_preceeding_nodes;
		}
	}
}

inline TraversedPathRecorder::TraversedPathRecorder() : root_node(nullptr)
{
}

inline void TraversedPathRecorder::Reset(TreeNode const* root_node)
{
	this->root_node = root_node;
	this->preceeding_nodes.clear();
}

inline void TraversedPathRecorder::AddRedirectNode(TreeNode * redirect_node)
{
	this->preceeding_nodes.push_back(redirect_node);
}

inline void TraversedPathRecorder::AddHiddenRedirectNode(TreeNode * redirect_parent, TreeNode * redirect_target)
{
	this->preceeding_nodes.push_back(redirect_parent);
	this->preceeding_nodes.push_back(redirect_target);
}

inline TraversedPathRecorderReverseIterator TraversedPathRecorder::GetReverseIterator(TreeNode const * leaf) const
{
	return TraversedPathRecorderReverseIterator(this->root_node, leaf, this->preceeding_nodes);
}
