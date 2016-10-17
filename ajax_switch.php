<?php

function float_rand($Min, $Max, $round=0){
    #validate input
    if ($Min > $Max) { $Min = $Max; $Max = $Min; }
        else { $Min = $Min; $Max = $Max; }
    $randomfloat = $Min + mt_rand() / mt_getrandmax() * ($Max - $Min);
    if($round > 0)
        $randomfloat = round($randomfloat,$round);
 
    return $randomfloat;
}
echo float_rand(0.01,5.00);
?>
