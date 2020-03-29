package buildyourownkotlinlisp

import org.antlr.v4.runtime.ParserRuleContext
import org.antlr.v4.runtime.tree.TerminalNode

abstract class ParseTreeElement {
    abstract fun multiLineString(indentation: String = ""): String
}

class ParseTreeLeaf(private val text: String) : ParseTreeElement() {
    override fun multiLineString(indentation: String): String = "${indentation}T[$text]\n"
    override fun toString(): String = "T$text"
}

class ParseTreeNode(private val name: String) : ParseTreeElement() {
    private val children = mutableListOf<ParseTreeElement>()

    fun child(c: ParseTreeElement): ParseTreeNode {
        children.add(c)
        return this
    }

    override fun multiLineString(indentation: String): String {
        val sb = StringBuilder()
        sb.append("$indentation$name\n")
        children.forEach {
            sb.append(it.multiLineString("$indentation "))
        }
        return sb.toString()
    }

    override fun toString(): String = "Node($name) $children"
}

fun toParseTree(node: ParserRuleContext): ParseTreeNode {
    val res = ParseTreeNode(node.javaClass.simpleName.removeSuffix("Context"))
    node.children.forEach { c ->
        when (c) {
            is ParserRuleContext -> res.child(toParseTree(c))
            is TerminalNode -> res.child(ParseTreeLeaf(c.text))
        }
    }
    return res
}
