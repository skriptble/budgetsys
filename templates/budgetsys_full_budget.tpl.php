<?php
  $content = $element->content;
?>
<table>
  <thead>
        <th>Account Title</th>
        <th>Account Number</th>
    <?php if(module_exists('budgetsys_org')) : ?>        
        <th>Organization</th>
    <?php endif; ?>     
        <th>Line Item Type</th>
    <?php foreach($content['years'] as $year) : ?>
        <th><?php print $year; ?></th>   
    <?php endforeach; ?>
  </thead>
  <tbody>
    <?php foreach($content['budget_lines'] as $content) : ?>
      <tr>
        <td><?php echo render($content['title']); ?></td>
        <td><?php echo render($content['account_number']); ?></td>
        <?php if(isset($content['organization'])) : ?>
        <td><?php echo render($content['organization']); ?></td>
        <?php endif; ?>
        <td><?php echo render($content['line_item_type']); ?></td>
      <?php foreach($content['values'] as $value) : ?>
        <td><?php print render($value); ?></td>   
      <?php endforeach; ?>      
      </tr>
    <?php endforeach; ?>
  </tbody>
  
</table>

  
