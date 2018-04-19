EXP=$1
RUNNO=$2

if [[ -z ${EXP} || -z ${RUNNO} ]]
then
  echo "./unpack.sh [Exp. folder name] [Run number]"

  exit 1
fi

./TCB_Conversion_root ../${EXP}/TCBRawData/RUN_${RUNNO}_tcb_bin.root unpackedData/RUN_${RUNNO}_tcb_conv.root
for iData in {0..3}
do
  for iLoop in {1..7}
  do
    printf -v iFADC "%d" $((iLoop+7*iData))
    ./FADC_Conversion_root ../${EXP}/Data${iData}/RUN_${RUNNO}_fadc_${iFADC}_bin.root unpackedData/RUN_${RUNNO}_fadc_${iFADC}_conv.root 4 ${RUNNO}
  done
done
