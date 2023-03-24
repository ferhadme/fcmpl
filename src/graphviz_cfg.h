#ifndef GRAPHVIZ_CFG_H
#define GRAPHVIZ_CFG_H

#define DOT_LAYOUT_ENGINE "dot"
#define DOT_GRAPH_FORMAT_FLAG "-Tsvg"
#define FILE_EXTENSION_LEN 5

#define DOT_FILE_ROOT_NODE_FORMAT "  \"%p\" [label=\"%c\";fillcolor=%s;style=filled;fontcolor=white]\n"
#define DOT_FILE_CHILD_NODE_FORMAT "  \"%p\" [label=\"%c\";fillcolor=%s;style=filled;fontcolor=white]\n"

#define ROOT_NODE_COLOR "red"
#define CHILD_NODE_COLOR "black"
#define EOW_CHILD_NODE_COLOR "green"

#endif // GRAPHVIZ_CFG_H
