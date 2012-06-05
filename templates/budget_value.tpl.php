<?php
  $content = $element->content;
  $bvid = isset($element->bvid) ? $bvid = $element->bvid : "";
?>
    <table>
      <tr>
        <th>Account Number</th>
        <td><?php echo render($content['account_number']); ?></td>
      </tr>
      <tr>
        <th>Account Value</th>
        <td><?php echo render($content['value']); ?></td>
      </tr>
      <tr>
        <th>Account Fiscal Year</th>
        <td><?php echo render($content['year']); ?></td>
      </tr>
      <tr>
        <th>Account Type</th>
        <td><?php echo render($content['type']); ?></td>
      </tr>
  <?php if(module_exists('budgetsys_org')) : ?>
       <tr>
        <th>Organization</th>
        <td><?php echo render($content['organization']); ?></td>
      </tr>   
    <?php endif; ?>
    </table>
