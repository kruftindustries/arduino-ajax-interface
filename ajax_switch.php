<?php

function float_rand($Min, $Max, $round=0){
    //validate input
    if ($min&gt;$Max) { $min=$Max; $max=$Min; }
        else { $min=$Min; $max=$Max; }
    $randomfloat = $min + mt_rand() / mt_getrandmax() * ($max - $min);
    if($round&gt;0)
        $randomfloat = round($randomfloat,$round);
 
    return $randomfloat;
}
echo float_rand(0.01,5.00);
?>
