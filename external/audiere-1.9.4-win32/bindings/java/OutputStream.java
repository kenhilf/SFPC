package org.aegisknight.audiere;

public class OutputStream {
  private OutputStream() {
  }
    
  protected native void finalize();

  public native void play();
  public native void stop();
  public native boolean isPlaying();
  public native void reset();

  public native boolean getRepeating();
  public native void setRepeating(boolean repeating);

  public native float getVolume();
  public native void setVolume(float volume);

  public native float getPan();
  public native void setPan(float pan);

  public native boolean isSeekable();
  public native int getLength();
  public native void setPosition(int position);
  public native int getPosition();

  // this is our internal pointer data...
  // evil evil evil  :(
  private long internal;
}
