<?php
//echo 'Lenght = '.round((sqrt(pow($X1_G - $X2_G, 2) + pow($Y1_G - $Y2_G, 2)))).'<br>';*/
header("Content-type: image/png");
session_start();
	class node {
	var $id;
	var $x;
	var $y;
	
	function node($id,$x,$y){
	$this->x=$x;
	$this->y=$y;
	$this->id=$id;
	}
	};
	
	class element {
	var $id;
	var $nodeIds;
	var $props;
	function element ($line){
	$this->nodeIds=array();
	$this->id=$line['id'];
	foreach($line as $x=>$x_value){
	if( strchr($x,"n")!=false) {
	array_push($this->nodeIds,$x_value);
	}
	}
	}
	};

	
	$width = 600;
	$height=600;
	$size=3;
	
	
	$link = mysql_connect("localhost", "rk6stud", "rk6stud")
	or die("Could not connect : " . mysql_error());
	mysql_select_db("femdb") or die("Could not select database");
	
	$Nodes = array();
	
	$nodesQuery = "SELECT id,x,y FROM nodes";
	$elementsQuery = "Select id,n1,n2,n3 from elements";
	$result = mysql_query($nodesQuery) or die("Query failed : " . mysql_error());
	
	
	while ($line = mysql_fetch_array($result, MYSQL_ASSOC)) {
	
	$NewNode=new node($line['id'],$line['x'],$line['y']);
	$Nodes+=array($line['id']=>$NewNode);
	}
	$result = mysql_query($elementsQuery) or die("Elemetns query is out" . mysql_error());
	$Elements=array();
	while($line = mysql_fetch_array($result,MYSQL_ASSOC)){
	$newElem = new element($line);
	array_push($Elements,$newElem);
	}
	
	mysql_free_result($result);
	
	mysql_close($link);
	
	$img = @imagecreate($width,$height)
	or die("Cannot Initialize new GD image stream");
	
	$background_color = imagecolorallocate($img, 255, 255, 255);
	$square_color = imagecolorallocate($img, 0, 0, 0);
	$text_color = imagecolorallocate($img, 233, 14, 91);
	
	foreach($Nodes as $id=>$femNode){
	$x1=($femNode->x+100)*$size;
	$y1=(-$femNode->y+100)*$size;
	$x2=$x1+3;
	$y2=$y1+3;
	imagefilledrectangle($img,$x1,$y1,$x2,$y2,$square_color);
	
	}
	
	
	foreach($Elements as $fem){
	
	$points=array();
	for($i=0;$i<3;$i++){
	$id = $fem->nodeIds[$i];
	$x=(int)($Nodes[$id]->x+101)*$size;
	$y=(int)(-$Nodes[$id]->y+101)*$size;
	array_push($points,$x);
	array_push($points,$y);
	}
	
	imagepolygon($img,$points,3,$square_color);
	unset($points);
	}
	
	
	if (isset($_SESSION['x'])) {
	$x=$_SESSION['x'];
	$y=$_SESSION['y'];
	$x=$_COOKIE["x"];
	$y=$_COOKIE["y"];
	$_SESSION['test_x']=$x;
	$_SESSION['test_y']=$y;
	$_SESSION['xP1']=$x;
	$_SESSION['yP1']=$y;
	$nearNode;
	$maxDelta=300;
	foreach($Nodes as $id=>$femNode){
	$x1=($femNode->x+100)*$size;
	$y1=(-$femNode->y+100)*$size;
	$delta=abs($x-$x1)+abs($y-$y1);
	if($delta<$maxDelta){
	$nearNode=$id;
	$maxDelta=$delta;
	}
	}
	$x1=(int)($Nodes[$nearNode]->x+101)*$size-10;
	$y1=(int)(-$Nodes[$nearNode]->y+101)*$size-10;
	$x2=$x1+20;
	$y2=$y1+20;
	//imagerectangle($img,$x1,$y1,$x2,$y2,$text_color);
	imagefilledrectangle($img,$x1,$y1,$x2,$y2,$text_color);
	//unset($_SESSION['count']);
	}
	
	if($_SESSION['count']>0 and $_SESSION['count']%2==0 ){
		
		if (isset($_SESSION['x1'])) {
	$x=$_SESSION['x1'];
	$y=$_SESSION['y1'];
	$x=$_COOKIE["x1"];
	$y=$_COOKIE["y1"];
	$_SESSION['test_x1']=$x;
	$_SESSION['test_y1']=$y;
	$_SESSION['xP']=$x;
	$_SESSION['yP']=$y;
	$nearNode;
	$maxDelta=300;
	foreach($Nodes as $id=>$femNode){
	$x1=($femNode->x+100)*$size;
	$y1=(-$femNode->y+100)*$size;
	$delta=abs($x-$x1)+abs($y-$y1);
	if($delta<$maxDelta){
	$nearNode=$id;
	$maxDelta=$delta;
	}
	}
	$x1=(int)($Nodes[$nearNode]->x+101)*$size-10;
	$y1=(int)(-$Nodes[$nearNode]->y+101)*$size-10;
	$x2=$x1+20;
	$y2=$y1+20;
	//imagerectangle($img,$x1,$y1,$x2,$y2,$text_color);
	imagefilledrectangle($img,$x1,$y1,$x2,$y2,$text_color);
	//imageline($img, $_SESSION['x1'], $_SESSION['y1'], $_SESSION['x2'], $_SESSION['y1'], $text_color);
	//unset($_SESSION['count']);
	}
		
		
		unset($_SESSION['count']);
	}
	if (isset($_SESSION['x']) and (isset($_SESSION['x1']))) {
		$x1=$_COOKIE["x"];
		$x2=$_COOKIE["x1"];
		$y1=$_COOKIE["y"];
		$y2=$_COOKIE["y1"];
		$scale_x = $width / 200;
		$scale_y = $height / 200;
		$X_0 = 100.0;
		$Y_0 = 100.0;
		$X1_G = ($x1 + $X_0) * $scale_x;
		$Y1_G = ($y1 + $Y_0) * $scale_y;
		$X2_G = ($x2 + $X_0) * $scale_x;
		$Y2_G = ($y2 + $Y_0) * $scale_y;
		imageline($img, $x1, $y1, $x2, $y2, $text_color);
		$l=round((sqrt(pow($X1_G - $X2_G, 2) + pow($Y1_G - $Y2_G, 2))));
		setcookie("Length", $l);
		
	}
	imagepng($img);
	imagedestroy($img);
	unset($_SESSION['x']);
	unset($_SESSION['y']);
	unset($_SESSION['x2']);
	unset($_SESSION['y2']);
	unset($_SESSION['test_x1']);
	unset($_SESSION['test_x']);
	unset($_SESSION['test_y1']);
	unset($_SESSION['test_y']);
	exit();
	?>
