{* Smarty *}

{config_load file='var.conf'}
{include file='header.tpl'}

{assign var="tds" value=array("Fist", "Last", "Address", "Hello", "sfds")}
<table border="{#tableBorderSize#}">
<tr bgcolor="{#rowBgColor#}">
    {foreach key=key item=item from=$tds}
    <td>{$item}</td>
    {/foreach}
    <td>{"now"|date_format:"%Y/%m/%d"}</td>
</tr>
</table>
<div id='showtest'></div>
{literal}
    <script language='javascript'>
    document.getElementById('showtest').innerHTML='Ha ha ha';
    </script>
{/literal}
{include file="footer.tpl"}
