<?php
  $content = $element->content;
  $lid = isset($element->lid) ? $lid = $element->lid : "";
?>
    <table>
      <thead>
        <th>Account Title</th>
        <th>Account Number</th>
        <th>Organization</th>
        <th>Line Item Type</th>
    <?php foreach($content['headers'] as $header) : ?>
        <th><?php print $header; ?></th>   
    <?php endforeach; ?>
      </thead>
      <tr>
        <td><?php echo render($content['title']); ?></td>
        <td><?php echo render($content['account_number']); ?></td>
        <td><?php echo render($content['organization']); ?></td>
        <td><?php echo render($content['line_item_type']); ?></td>

      <?php foreach($content['values'] as $value) : ?>
        <td><?php print render($value); ?></td>   
      <?php endforeach; ?>          
      </tr>      
    </table>
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
       <?php for($l=0;$l<count($content['values']);$l++) : ?>
        <th><?php print $content['headers'][$l];?></th>
        <td><?php print render($content['values'][$l]); ?></td>
      </tr>
      <?php endfor; ?>
    </table>