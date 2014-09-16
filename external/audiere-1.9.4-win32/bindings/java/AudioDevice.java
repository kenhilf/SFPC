package org.aegisknight.audiere;

public class AudioDevice {
  static {
    System.loadLibrary("javaAudiere");
  }

  public AudioDevice() throws AudiereException {
    initialize("", "");
  }

  public AudioDevice(String name) throws AudiereException {
    initialize(name, "");
  }

  public AudioDevice(String name, String parameters) throws AudiereException {
    initialize(name, parameters);
  }

  private native void initialize(String name, String parameters)
    throws AudiereException;
  protected native void finalize();

  public native OutputStream createTone(double frequency)
    throws AudiereException;
  public native OutputStream openSound(String filename, boolean streaming)
    throws AudiereException;

  // this is our internal pointer data...
  // evil evil evil  :(
  private long internal;
}
