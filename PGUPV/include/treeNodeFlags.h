#pragma once

namespace PGUPV {
	using TreeNodeFlags = unsigned int;
	enum class TreeNodeFlag : unsigned int {
		TreeNodeFlags_None = 0,
		TreeNodeFlags_Selected = 1 << 0,   // Draw as selected
		TreeNodeFlags_Framed = 1 << 1,   // Full colored frame (e.g. for CollapsingHeader)
		TreeNodeFlags_AllowItemOverlap = 1 << 2,   // Hit testing to allow subsequent widgets to overlap this one
		TreeNodeFlags_NoTreePushOnOpen = 1 << 3,   // Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
		TreeNodeFlags_NoAutoOpenOnLog = 1 << 4,   // Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
		TreeNodeFlags_DefaultOpen = 1 << 5,   // Default node to be open
		TreeNodeFlags_OpenOnDoubleClick = 1 << 6,   // Need double-click to open node
		TreeNodeFlags_OpenOnArrow = 1 << 7,   // Only open when clicking on the arrow part. If ImGuiTreeNodeFlags_OpenOnDoubleClick is also set, single-click arrow or double-click all box to open.
		TreeNodeFlags_Leaf = 1 << 8,   // No collapsing, no arrow (use as a convenience for leaf nodes).
		TreeNodeFlags_Bullet = 1 << 9,   // Display a bullet instead of arrow
		TreeNodeFlags_FramePadding = 1 << 10,  // Use FramePadding (even for an unframed text node) to vertically align text baseline to regular widget height. Equivalent to calling AlignTextToFramePadding().
		TreeNodeFlags_NavLeftJumpsBackHere = 1 << 13,  // (WIP) Nav: left direction may move to this TreeNode() from any of its child (items submitted between TreeNode and TreePop)

		TreeNodeFlags_PGUPV_Visible = 1 << 15,
		TreeNodeFlags_PGUPV_Invisible = 1 << 16,
		TreeNodeFlags_PGUPV_Movie = 1 << 17,

		TreeNodeFlags_CollapsingHeader = TreeNodeFlags_Framed | TreeNodeFlags_NoTreePushOnOpen | TreeNodeFlags_NoAutoOpenOnLog
	};
};