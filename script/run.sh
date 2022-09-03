#!/bin/bash
if [ ! -f "../Release/spatium-mock-speech" ]; then
echo Build before run please.
exit
fi

if [ ! -d "../Run" ]; then
mkdir ../Run
cp ../conf/* ../Run
fi

rm ../Run/spatium-mock-speech
cp ../Release/spatium-mock-speech ../Run
cd ../Run

./spatium-mock-speech
