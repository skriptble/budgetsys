<?php
  $content = $element->content;
  $aid = isset($element->aid) ? $aid = $element->aid : "";
?>

<?php if (user_is_logged_in()) : ?>
  <p style="float: right;"><a href="?q=budget_value/<?php echo $aid; ?>/edit">Edit</a></p>
  <?php endif; ?>
  <?php echo render($content['title']); ?>
  <p class="meta">
    <?php echo render($content['account_number']); ?><br />
    <?php echo render($content['value']); ?><br />
    <?php echo render($content['year']); ?><br />
      </p>
    <table>
      <tr>
        <th>Account Number</th>
        <th>Account Value</th>
        <th>Account Year</th>
      </tr>
      <tr>
        <th><?php echo render($content['account_number']); ?></th>
        <td><?php echo render($content['value']); ?></td>
        <td><?php echo render($content['year']); ?></td>
      </tr>
    </table>
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
    </table>
