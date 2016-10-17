<?php

#Generate random analog voltage value in the same format as the Arduino
function float_rand($Min, $Max, $round=0){
    #validate input
    if ($Min > $Max) { $Min = $Max; $Max = $Min; }
        else { $Min = $Min; $Max = $Max; }
    $randomfloat = $Min + mt_rand() / mt_getrandmax() * ($Max - $Min);
    if($round > 0)
        $randomfloat = round($randomfloat,$round);
 
    return $randomfloat;
}

#Display digital and analog values!
echo "<p>Switch 7 state: ";
#generate random bool!
if (rand(0, 1)) { echo "ON</p>"; } else {echo "OFF</p>";}
echo "<p>Switch 8 state: ";
if (rand(0, 1)) { echo "ON</p>"; } else {echo "OFF</p>";}

echo float_rand(0.01,5.00);
?>
