<?php
  $content = $element->content;
  $oid = isset($element->oid) ? $oid = $element->oid : "";
  drupal_set_title($content['org_title']);
?>

    <table>
      <thead>
        <th>Account Title</th>
        <th>Account Number</th>
        <th>Line Item Type</th>
    <?php foreach($content['headers'] as $year) : ?>
        <th><?php print $year; ?></th>   
    <?php endforeach; ?>
      </thead>
    <?php for($d=0;$d<$content['delta'];$d++) : ?>
      <tr>
        <td><?php echo render($content['title'][$d]); ?></td>
        <td><?php echo render($content['account_number'][$d]); ?></td>
        <td><?php echo render($content['line_item_type'][$d]); ?></td>

      <?php foreach($content['values'][$d] as $value) : ?>
        <td><?php print render($value); ?></td>   
      <?php endforeach; ?>       
      </tr>
    <?php endfor; ?>        
    </table>
