<?php
  $content = $element->content;
?>
<?php foreach($content['organizations'] as $organization) : ?>
<h1><?php print $organization['org_title']; ?></h1>

    <table>
      <thead>
        <th>Account Title</th>
        <th>Account Number</th>
        <th>Line Item Type</th>
    <?php foreach($organization['years'] as $year) : ?>
        <th><?php print $year; ?></th>   
    <?php endforeach; ?>
      </thead>
    <?php for($d=0;$d<$organization['delta'];$d++) : ?>
      <tr>
        <td><?php echo render($organization['title'][$d]); ?></td>
        <td><?php echo render($organization['account_number'][$d]); ?></td>
        <td><?php echo render($organization['line_item_type'][$d]); ?></td>

      <?php foreach($organization['values'][$d] as $value) : ?>
        <td><?php print render($value); ?></td>   
      <?php endforeach; ?>       
      </tr>
    <?php endfor; ?>        
    </table>
<?php endforeach; ?>