<?php
	
	$power= $_POST['power'];

	$fileContent=$power."\n";
	
	$fileStatus = file_put_contents('powerData.txt',$fileContent,FILE_APPEND);
	
	if($fileStatus != false){
		echo "Wrote to file";
	}else{
		echo "Could not write to file";
	}
?>
