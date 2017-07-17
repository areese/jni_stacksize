JNI Stack Size example
=======
 
Overview
-----------
Stack clash showed an issue with the jvm based on stacksize.
Stacksize as passed by -Xss isn't visible via mbeans or via Runtime as far as I can tell
This library calls pthread_attr_getstacksize and returns it's value.
I wrote it as a fork of jsvc might not honour -Xss correctly


Running
-----------


Internals
It turns out this is not a trivial procedure.



References
-----------
http://docs.oracle.com/javase/7/docs/api/java/nio/ByteBuffer.html

https://www.linkedin.com/profile/view?id=1541345&authType=NAME_SEARCH&authToken=oafT&locale=en_US&srchid=80459361437146014650&srchindex=1&srchtotal=334&trk=vsrp_people_res_name&trkInfo=VSRPsearchId%3A80459361437146014650%2CVSRPtargetId%3A1541345%2CVSRPcmpt%3Aprimary%2CVSRPnm%3Atrue

