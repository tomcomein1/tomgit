<?php /* Smarty version Smarty-3.1.13, created on 2017-06-20 14:31:03
         compiled from "smarty\templates\index.tpl" */ ?>
<?php /*%%SmartyHeaderCode:308065948a5379b30f2-95141922%%*/if(!defined('SMARTY_DIR')) exit('no direct access allowed');
$_valid = $_smarty_tpl->decodeProperties(array (
  'file_dependency' => 
  array (
    'b389985f94f94bc42e54c5354e6f94774adfbf04' => 
    array (
      0 => 'smarty\\templates\\index.tpl',
      1 => 1497940259,
      2 => 'file',
    ),
  ),
  'nocache_hash' => '308065948a5379b30f2-95141922',
  'function' => 
  array (
  ),
  'cache_lifetime' => 3600,
  'version' => 'Smarty-3.1.13',
  'unifunc' => 'content_5948a537a17106_53984013',
  'variables' => 
  array (
    'tds' => 0,
    'item' => 0,
  ),
  'has_nocache_code' => false,
),false); /*/%%SmartyHeaderCode%%*/?>
<?php if ($_valid && !is_callable('content_5948a537a17106_53984013')) {function content_5948a537a17106_53984013($_smarty_tpl) {?><?php if (!is_callable('smarty_modifier_date_format')) include 'libs/Smarty-3.1.13/libs/plugins\\modifier.date_format.php';
?>

<?php  $_config = new Smarty_Internal_Config('var.conf', $_smarty_tpl->smarty, $_smarty_tpl);$_config->loadConfigVars(null, 'local'); ?>
<?php echo $_smarty_tpl->getSubTemplate ('header.tpl', $_smarty_tpl->cache_id, $_smarty_tpl->compile_id, 9999, null, array(), 0);?>


<table border="<?php echo $_smarty_tpl->getConfigVariable('tableBorderSize');?>
">
<tr bgcolor="<?php echo $_smarty_tpl->getConfigVariable('rowBgColor');?>
">
    <?php  $_smarty_tpl->tpl_vars['item'] = new Smarty_Variable; $_smarty_tpl->tpl_vars['item']->_loop = false;
 $_smarty_tpl->tpl_vars['key'] = new Smarty_Variable;
 $_from = $_smarty_tpl->tpl_vars['tds']->value; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array');}
foreach ($_from as $_smarty_tpl->tpl_vars['item']->key => $_smarty_tpl->tpl_vars['item']->value){
$_smarty_tpl->tpl_vars['item']->_loop = true;
 $_smarty_tpl->tpl_vars['key']->value = $_smarty_tpl->tpl_vars['item']->key;
?>
    <td><?php echo $_smarty_tpl->tpl_vars['item']->value;?>
</td>
    <?php } ?>
    <td><?php echo smarty_modifier_date_format("now","%Y/%m/%d");?>
</td>
</tr>
</table>
<?php echo $_smarty_tpl->getSubTemplate ("footer.tpl", $_smarty_tpl->cache_id, $_smarty_tpl->compile_id, 9999, null, array(), 0);?>

<?php }} ?>