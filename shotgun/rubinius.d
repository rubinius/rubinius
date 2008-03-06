provider rubinius {
    /* Method entry/exit probes*/
    probe function__entry(char*, char*, char*, int);
    probe function__return(char*, char*, char*, int);
    
    /* Primitive operations entry/exit probes */
    probe function__primitive__entry(char*, char*, char*, int);
    probe function__primitive__return(char*, char*, char*, int);
    
    /* gc probes */
    probe gc__begin();
    probe gc__end();
    
    /* Some initial memory type probes */
    probe object__create__start(char*, char*, int);
    probe object__create__done(char*, char*, int);

    /* internal time probes */
    probe vm__lookup__begin();
    probe vm__lookup__end();
    probe vm__send__begin();
    probe vm__send__end();
    probe vm__context__create__begin();
    probe vm__context__create__end();
    
    /* ruby application probe */
    probe ruby__probe(char*, char*);
};

#pragma D attributes Evolving/Evolving/Common provider rubinius provider
#pragma D attributes Private/Private/Common provider rubinius module
#pragma D attributes Private/Private/Common provider rubinius function
#pragma D attributes Evolving/Evolving/Common provider rubinius name
#pragma D attributes Evolving/Evolving/Common provider rubinius args
