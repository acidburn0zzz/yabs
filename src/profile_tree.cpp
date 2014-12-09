#include "profile_tree.h"

ProfileNode::ProfileNode()
{
	is_leaf = false;
	child_count = 0;
	Child = NULL;
	Parent = NULL;
	Children.clear();
	node_value = '\0';
}

ProfileNode::~ProfileNode() {}

void ProfileNode::AddChild(std::string ChildValue)
{
	Children.push_back(new ProfileNode());
	this->Child = Children[child_count];
	Children[child_count]->node_value = ChildValue;
	Children[child_count]->Parent = this;
	Children[child_count]->is_leaf = true;
	this->child_count++;
}

bool ProfileNode::HasChild()
{
	if (Child == NULL) {
		printf("Node has no child\n");
		return false;
	} else {
		printf("Node has a child\n");
		return true;
	}
}

void ProfileNode::RemoveAllChildren()
{
	if (!Children.empty()) {
		for (int i = 0; i < (int)Children.size(); i++) {
			delete Children[i];
		}
	}
}

std::string ProfileNode::GetParentValue() const
{
	if (Parent != NULL) {
		return Parent->node_value;
	} else {
		printf("Node has no parent\n");
	}
	return NULL;
}

std::string ProfileNode::GetLastChildValue() const
{
	if (!Children.empty())
		return Children[child_count]->node_value;
	else
		printf("No last child\n");

	return NULL;
}

ProfileTree::ProfileTree() {}
ProfileTree::~ProfileTree() {}

void ProfileTree::AddTreeChild(std::string Value)
{
	Root.AddChild(Value);
}

void ProfileTree::GetNodeParentValue()
{
	Root.GetParentValue();
}
