provider rubinius {
    probe function__entry(char*, char*, char*, int);
    probe function__return(char*, char*, char*, int);
    
    /* gc probes */
    probe gc__begin();
    probe gc__end();
    
    /* Some initial memory type probes */
    probe object__create__start(char*, char*, int);
    probe object__create__done(char*, char*, int);
};

#pragma D attributes Evolving/Evolving/Common provider rubinius provider
#pragma D attributes Private/Private/Common provider rubinius module
#pragma D attributes Private/Private/Common provider rubinius function
#pragma D attributes Evolving/Evolving/Common provider rubinius name
#pragma D attributes Evolving/Evolving/Common provider rubinius args
