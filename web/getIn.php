<?php

$val = $_GET["power"];

$fileContent=$val."\n";

$fileStatus=file_put_contents("datastorage.txt",$fileContent,FILE_APPEND);

if($fileStatus != false){
	echo  "SUCCESS. Data written in file.";
}else{
	echo  "FAIL. Could not connect to file.";
}

?>
