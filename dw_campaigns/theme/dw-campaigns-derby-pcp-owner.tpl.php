<?php
global $user;

if($user->uid === $thisUser->uid) {
?>
	<ul class="links">
		<li><a href="/dw/user/edit_page"><?php echo t('Edit My Fundraising Page'); ?></a></li>
	</ul>
<?php
} else {
	
}
