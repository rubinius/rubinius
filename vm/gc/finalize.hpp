namespace rubinius {
  struct FinalizeObject {
  public:
    enum FinalizationStatus {
      eLive,
      eQueued,
      eFinalized
    };

  public:
    Object* object;
    FinalizationStatus status;
  };
}
