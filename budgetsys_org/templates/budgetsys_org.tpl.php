<?php
  $content = $element->content;
  $oid = isset($element->oid) ? $oid = $element->oid : "";
?>
    <table>
      <tr>
        <th>Active</th>
        <td><?php echo render($content['active']); ?></td>
      </tr>
      <tr>
        <th>Budget Category</th>
        <td><?php echo render($content['budget_category']); ?></td>
      </tr>
    </table>
