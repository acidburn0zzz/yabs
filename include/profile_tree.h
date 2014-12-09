#ifndef _PROF_NODE_H
#define _PROF_NODE_H

#include <vector>
#include <string>
#include <stdio.h>

class ProfileNode
{
public:
	std::string node_value;
	bool is_leaf;
	int child_count;
	ProfileNode *Parent;
	ProfileNode *Child;
	std::vector<ProfileNode *> Children;
	ProfileNode();
	~ProfileNode();
	std::string GetLastChildValue() const;
	std::string GetParentValue() const;
	ProfileNode *GetParent();
	void AddChild(std::string ChildValue);
	void RemoveAllChildren();
	bool HasChild();
};

class ProfileTree
{
public:
	ProfileTree();
	~ProfileTree();
	ProfileNode Root;
	void NewRoot(ProfileTree&, ProfileNode*, ProfileNode*);
	void AddTreeChild(std::string Value);
	void GetNodeParentValue();
	void Pop();
};

#endif
