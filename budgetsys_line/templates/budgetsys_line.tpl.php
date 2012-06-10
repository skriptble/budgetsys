<?php
  $content = $element->content;
  $lid = isset($element->lid) ? $lid = $element->lid : "";
?>
    <table>
      <tr>
        <th>Account Title</th>
        <td><?php echo render($content['title']); ?></td>
      </tr>
      <tr>
        <th>Account Number</th>
        <td><?php echo render($content['account_number']); ?></td>
      </tr>
      <tr>
        <th>Organization</th>
        <td><?php echo render($content['organization']); ?></td>
      </tr>
      <tr>
        <th>Line Item Type</th>
        <td><?php echo render($content['line_item_type']); ?></td>
      </tr>      
    </table>
