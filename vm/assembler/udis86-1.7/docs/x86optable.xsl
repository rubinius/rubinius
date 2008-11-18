<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0" >
<xsl:template match="x86optable">
<html>
   <head>
    <title>x86 opcode table</title>
    <style>
    .mnm {
       border-bottom: 1px dotted #cdcdcd;
       font-family: "lucida sans", georgia, helvetica, arial, verdana, georgia;
       border-right: 1px solid #cdcdcd;
       font-size: 1em;
    }

    .opc {
       border-bottom: 1px dotted #cdcdcd;
       border-right: 1px solid #cdcdcd;
       font-family: monospace;
       font-size: 1.1em;
    }

    .vdr {
       border-bottom: 1px dotted #cdcdcd;
       border-right: 1px solid #cdcdcd;
       font-size: .9em;
    }      
    </style>
   </head>
   <body> 
      <h1 style="text-align:left; padding-left:8px;">x86optable.xml</h1>
	<p style="text-align:left; padding-left:8px">
		<a style="text-decoration:none" href="http://udis86.sourceforge.net/">udis86.sourceforge.net</a></p>
      <table cellpadding="4" cellspacing="6" width="800px"> 
         <tr bgcolor='lightblue'>
            <td align="center">Mnemonic</td>
	    <td align="center">Opcodes</td>
	    <td align="center">Vendor</td>
         </tr>
         <xsl:for-each select="instruction">
            <tr>   
		<td class="mnm" align="center" valign="middle"><xsl:value-of select="@mnemonic"/></td>
		<td class="opc">
		<xsl:for-each select="opcode">
		<xsl:apply-templates/>	
		<br/>
		</xsl:for-each>
		</td>
		<td class="vdr" align="center" valign="top">
		<xsl:for-each select="vendor">
		<xsl:apply-templates/>	
		<br/>
		</xsl:for-each>
		</td>
            </tr>
         </xsl:for-each>
      </table>
	<p style="text-align:left; padding-left:8px">
		<small>Copyright (c) 2008, Vivek Thampi</small>
	</p>

   </body>
</html>
</xsl:template>
</xsl:stylesheet>
