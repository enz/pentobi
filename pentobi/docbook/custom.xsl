<?xml version='1.0'?>
<xsl:stylesheet
xmlns:xsl="http://www.w3.org/1999/XSL/Transform"  version="1.0">
<xsl:import href="chunk.xsl"/>
<xsl:param name="admon.graphics" select="0"/>
<xsl:param name="chapter.autolabel" select="0"/>
<xsl:param name="chunk.quietly" select="1"/>
<xsl:param name="chunk.section.depth" select="0"/>
<xsl:param name="chunker.output.encoding" select="'UTF-8'"/>
<xsl:param name="chunker.output.indent" select="'yes'"/>
<xsl:param name="footer.rule" select="1"/>
<xsl:param name="generate.index" select="1"/>
<xsl:param name="generate.toc" select="'book toc'"/>
<xsl:param name="header.rule" select="1"/>
<xsl:param name="html.cleanup" select="1"/>
<xsl:param name="html.stylesheet" select="'../../C/pentobi/stylesheet.css'"/>
<xsl:param name="make.valid.html" select="1"/>
<xsl:param name="refentry.generate.name" select="0"/>
<xsl:param name="refentry.generate.title" select="1"/>
<xsl:param name="spacing.paras" select="0"/>
<xsl:param name="toc.section.depth" select="0"/>
<xsl:param name="use.id.as.filename" select="1"/>

<xsl:template name="header.navigation">
<xsl:param name="prev" select="/foo"/>
<xsl:param name="next" select="/foo"/>
<xsl:param name="nav.context"/>
<xsl:variable name="home" select="/*[1]"/>
<div class="navheader">
<table width="100%" summary="Navigation header">
<tr>
<td width="40%" align="{$direction.align.start}">
<xsl:if test="count($prev)>0">
<a accesskey="p">
<xsl:attribute name="href">
<xsl:call-template name="href.target">
<xsl:with-param name="object" select="$prev"/>
</xsl:call-template>
</xsl:attribute>
<xsl:call-template name="navig.content">
<xsl:with-param name="direction" select="'prev'"/>
</xsl:call-template>
</a>
</xsl:if>
<xsl:text>&#160;</xsl:text>
</td>
<td width="20%" align="center">
<xsl:choose>
<xsl:when test="$home != . or $nav.context = 'toc'">
<a accesskey="h">
<xsl:attribute name="href">
<xsl:call-template name="href.target">
<xsl:with-param name="object" select="$home"/>
</xsl:call-template>
</xsl:attribute>
<xsl:call-template name="navig.content">
<xsl:with-param name="direction" select="'home'"/>
</xsl:call-template>
</a>
</xsl:when>
<xsl:otherwise>&#160;</xsl:otherwise>
</xsl:choose>
</td>
<td width="40%" align="{$direction.align.end}">
<xsl:text>&#160;</xsl:text>
<xsl:if test="count($next)>0">
<a accesskey="n">
<xsl:attribute name="href">
<xsl:call-template name="href.target">
<xsl:with-param name="object" select="$next"/>
</xsl:call-template>
</xsl:attribute>
<xsl:call-template name="navig.content">
<xsl:with-param name="direction" select="'next'"/>
</xsl:call-template>
</a>
</xsl:if>
</td>
</tr>
</table>
<hr/>
</div>
</xsl:template>

<xsl:template name="footer.navigation">
<xsl:param name="prev" select="/foo"/>
<xsl:param name="next" select="/foo"/>
<xsl:param name="nav.context"/>
<xsl:variable name="home" select="/*[1]"/>
<div class="navheader">
<hr/>
<table width="100%" summary="Navigation footer">
<tr>
<td width="40%" align="{$direction.align.start}">
<xsl:if test="count($prev)>0">
<a accesskey="p">
<xsl:attribute name="href">
<xsl:call-template name="href.target">
<xsl:with-param name="object" select="$prev"/>
</xsl:call-template>
</xsl:attribute>
<xsl:call-template name="navig.content">
<xsl:with-param name="direction" select="'prev'"/>
</xsl:call-template>
</a>
</xsl:if>
<xsl:text>&#160;</xsl:text>
</td>
<td width="20%" align="center">
<xsl:choose>
<xsl:when test="$home != . or $nav.context = 'toc'">
<a accesskey="h">
<xsl:attribute name="href">
<xsl:call-template name="href.target">
<xsl:with-param name="object" select="$home"/>
</xsl:call-template>
</xsl:attribute>
<xsl:call-template name="navig.content">
<xsl:with-param name="direction" select="'home'"/>
</xsl:call-template>
</a>
</xsl:when>
<xsl:otherwise>&#160;</xsl:otherwise>
</xsl:choose>
</td>
<td width="40%" align="{$direction.align.end}">
<xsl:text>&#160;</xsl:text>
<xsl:if test="count($next)>0">
<a accesskey="n">
<xsl:attribute name="href">
<xsl:call-template name="href.target">
<xsl:with-param name="object" select="$next"/>
</xsl:call-template>
</xsl:attribute>
<xsl:call-template name="navig.content">
<xsl:with-param name="direction" select="'next'"/>
</xsl:call-template>
</a>
</xsl:if>
</td>
</tr>
</table>
</div>
</xsl:template>

</xsl:stylesheet>
