<?php
  $content = $element->content;
  $categories = $content['categories'];
?>
<?php foreach($categories as $category) : ?>
<h1><?php print $category; ?></h1>
<?php foreach($content[$category]['organizations'] as $organization) : ?>
<?php $colspan = 3 + count($organization['headers']);?>
    <table>
      <thead>
        <th colspan="<?php print $colspan; ?>"><?php print $organization['org_title']; ?></th>
      </thead>
      <thead>
        <th>Account Title</th>
        <th>Account Number</th>
        <th>Line Item Type</th>
    <?php foreach($organization['headers'] as $header) : ?>
        <th><?php print $header; ?></th>   
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
<?php endforeach; ?>