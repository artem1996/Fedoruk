<?php
	session_start();
	$width = 600;
	$height=600;
	$size=3;
	if(!isset($_SESSION['count'])){
		
	if ($_SERVER['REQUEST_METHOD'] === 'POST') {
			
			$_SESSION['y']=$_POST['sub_y'];
			$_SESSION['x']=$_POST['sub_x'];
			echo "x and y: ";
			echo $_SESSION['y']." ";
			echo $_SESSION['x']."  ";
			$_SESSION['count']=$_SESSION['count']+1;
			echo $_SESSION['count'];
			setcookie("y", $_POST['sub_y']);
			setcookie("x", $_POST['sub_x']);
		}
	}else{
			$_SESSION['y1']=$_POST['sub_y'];
			$_SESSION['x1']=$_POST['sub_x'];
			setcookie("y1", $_POST['sub_y']);
			setcookie("x1", $_POST['sub_x']);
			echo "x1 and y1: ";
			echo $_SESSION['y1']." ";
			echo $_SESSION['x1']."  ";
			$_SESSION['count']=$_SESSION['count']+1;
			echo $_SESSION['count'];
			
	}
	
	print "<FORM ACTION='index.php' method='post'>";
	echo "<input type='image' src='ig.php' name='sub' alt='Image' style='width:${width}px;height:${height};'>";
	print "</FORM>";
	echo "NEw ";
	echo $_SESSION['test_x1']."  ";
	echo $_SESSION['test_y1'];
	
	echo "NEw ";
	echo $_SESSION['test_x']."  ";
	echo $_SESSION['test_y'];
	
	echo "new2"."    ";
	echo $_SESSION['xP']."   ";
	echo $_SESSION['yP'];
	echo "new2"."    ";
	echo $_SESSION['xP1']."   ";
	echo $_SESSION['yP1'];
	
	echo "Answer:";
	echo $_COOKIE["Length"];
	//session_destroy();
	?>
