<?php 

$user = $thisUser;
// shamelessly taken from fpwr project
/**
 * If they are not logged in, show custom login form, this will be handled in the init 
 * of our fpwr_contributions module. Opted to go with custom markup so it's easier to
 * match the psd provided by jeff (and who likes fighting with drupals form api anyway)
 */

	if($user->uid == 0) { 

    /**
     * All this is required so drupal will call dw_campaigns_user_login_submit()
     */
    $form_id                    = 'dw_campaigns_user_login_form';
    $form                       = dw_campaigns_user_login_form();
    $form_build_id              = 'form-'. md5(uniqid(mt_rand(), true));
    $form['#build_id'] 	= $form_build_id;
    if(count($_POST) > 0) {
        $form['#post']		= $_POST;
    }
    
    $form_state 		= array('storage' => NULL, 'submitted' => FALSE);
            
    drupal_prepare_form($form_id, $form, $form_state);
    drupal_process_form($form_id, $form, $form_state);

    //remove labels
    unset($form['name']['#title']);
    unset($form['pass']['#title']); 

?>
    <h2><?php echo t('Fundraiser Login'); ?></h2>
	<?php
		if(isset($_REQUEST['create'])) {
			echo '<p>' . t('You must have an account to create a fundraising page.') . '</p>';
                        echo '<p>' . t('Please login below or create an account now ');
                        if($mode_type == 'walking') {
                            echo l(t('Sign Up'), 'dw/user/register_oss');
                        } else {
                            echo l(t('Sign Up'), 'dw/user/register');
                        }
                        echo '</p>';
		}
	?>
    <form class="login" action="<?php echo request_uri(); ?>" method="post">
        <ul>
            <li>
            	<?php echo drupal_render($form['name']); ?>
            </li>
            <li>
            	<?php echo drupal_render($form['pass']); ?>
            </li>
            <li class="submit"><button type="submit" ><?php echo t('Login'); ?></button></li>
        </ul>
		<div class="forgot-signup">
			<a href="/user/password"><?php echo t('Password Reminder'); ?></a>
		<?php
		if(!isset($_REQUEST['create'])) {
		?>
			<?php echo t('No Account Yet?'); ?> <?php echo l(t('Sign Up'), 'dw/user/register');?>
        <?php
		} 
	?>
		</div>    
		<?php
            echo drupal_render($form['form_id']);
            echo drupal_render($form['form_build_id']);
        ?>
    </form>
<?php } else { ?>
    <h2><?php echo t('Controls'); ?></h2>
    <div class="user-controls">
    	<a href="/logout?destination=dw" class="btn"><?php echo t('Logout'); ?></a>
    </div>
<?php } ?>
