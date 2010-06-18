import os
# python script to process several test jobs, author E. von Toerne

#setupstring='export LD_LIBRARY_PATH=/usr/cern/root/lib;cd ../../;source setup.sh;cd test/DataInput;'
setupstring=''
basedir = './'
os.system('rm -rf results;mkdir results')

indxs = [0,1,2,3,4,5,6,7,8,10,11,12,13,14,15,16,17,18,20,30,31,32,33,40,41,42,50,51,52,53,54,60,70,71,100,101,102,103,104,105,106,107,108,120,121,122,123,124,125,130,131]
for index in indxs:
    print "index",index
    logfile="results/TMVATestDataInput%d.log"%(index)
    rootfile="TMVATestDataInput%d.root"%(index)
    #os.chdir(basedir)
    os.system(setupstring+'./TMVATestDataInput %d'%(index)+" >"+logfile+";mv %s results/."%rootfile)

