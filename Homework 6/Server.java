import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.lang.Runnable;
import java.util.Scanner;
import java.util.Random;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import javax.crypto.Cipher;
import javax.crypto.SecretKey;
import javax.crypto.KeyGenerator;
import javax.crypto.spec.SecretKeySpec;
import java.security.PublicKey;
import java.security.KeyFactory;
import java.security.spec.X509EncodedKeySpec;
import java.security.GeneralSecurityException;
import java.util.Base64;

/** A server that keeps up with a public key for every user, scrabble
 * word scores for all users. */
public class Server{


  //private static Object for reporting
  private static Object lock = new Object();

  // //private static Object for submitting
  // private static Object submitting = new Object();

  /** Esentially, the main method for our server, as an instance method
      so we can access non-static fields. */
      private void run( String[] args ) {
        ServerSocket serverSocket = null;
        
        // One-time setup.
        try {
          // Read the map and the public keys for all the users.
          readUsers();
    
          // Open a socket for listening.
          serverSocket = new ServerSocket( PORT_NUMBER );
        } catch( Exception e ){
          System.err.println( "Can't initialize server: " + e );
          e.printStackTrace();
          System.exit( 1 );
        }
         
        // Keep trying to accept new connections and serve them.
        while( true ){
          try {
            // Try to get a new client connection.
            Socket sock = serverSocket.accept();
    
            // Hande interaction with this client.
            //handleClient( sock );
            ClientHandler clientSock
                        = new ClientHandler(sock);
            new Thread(clientSock).start();            
          } catch( IOException e ){
            System.err.println( "Failure accepting client " + e );
          }
        }
      }
    
      public static void main( String[] args ) {
        // Make a server object, so we can use non-static fields and methods.
        Server server = new Server();
        server.run( args );
      }


  private class ClientHandler implements Runnable{
    /** Handle interaction with our client. */
    private Socket sock;

    ClientHandler( Socket sock ) {
      this.sock = sock;
    }

    public void run(){
      try {
        // Get formatted input/output streams for this thread.  These can read and write
        // strings, arrays of bytes, ints, lots of things.
        DataOutputStream output = new DataOutputStream( sock.getOutputStream() );
        DataInputStream input = new DataInputStream( sock.getInputStream() );
        
        // Get the username.
        String username = input.readUTF();

        // Make a random sequence of bytes to use as a challenge string.
        Random rand = new Random();
        byte[] challenge = new byte [ 16 ];
        rand.nextBytes( challenge );

        // Make a session key for communiating over AES.  We use it later, if the
        // client successfully authenticates.
        byte[] sessionKey = new byte [ 16 ];
        rand.nextBytes( sessionKey );

        // Find this user.  We don't need to synchronize here, since the set of users never
        // changes.
        UserRec rec = null;
        for ( int i = 0; rec == null && i < userList.size(); i++ )
          if ( userList.get( i ).name.equals( username ) )
            rec = userList.get( i );

        // Did we find a record for this user?
        if ( rec != null ) {
          // Make sure the client encrypted the challenge properly.
          Cipher RSADecrypter = Cipher.getInstance( "RSA" );
          RSADecrypter.init( Cipher.DECRYPT_MODE, rec.publicKey );
            
          Cipher RSAEncrypter = Cipher.getInstance( "RSA" );
          RSAEncrypter.init( Cipher.ENCRYPT_MODE, rec.publicKey );
            
          // Send the client the challenge.
          putMessage( output, challenge );
            
          // Get back the client's encrypted challenge.
          byte[] buffer = new byte[64];
          int len = input.read( buffer );

          // Make sure the client properly encrypted the challenge.
          byte[] test = RSADecrypter.doFinal( buffer );
          if(!Arrays.equals(test, challenge)){
            System.err.println( "Challenges are not equal" );
            System.exit( 1 );
          }
          // else{
          //   System.out.println("Challenges are equal!");
          // }

          // Send the client our session key (encrypted)
          // ...
          // byte[] sesKey = new SecretKeySpec( rawKey, "AES" );
          // KeyGenerator keygen = KeyGenerator.getInstance("AES");
          // keygen.init(128);
          // byte[] sesKey = keygen.generateKey().getEncoded();
          byte[] sessionEncrypted = RSAEncrypter.doFinal(sessionKey);
          //System.out.println(sessionEncrypted);
          putMessage( output, sessionEncrypted );
          // output.write(sessionEncrypted, 0, sessionEncrypted.length);
          // output.flush();
          
          // Make AES cipher objects to encrypt and decrypt with
          // the session key.

          // KeyGenerator kgen = KeyGenerator.getInstance("AES");
          // kgen.init(128); // 192 and 256 bits may not be available
          SecretKeySpec AESpublicKey = new SecretKeySpec(sessionKey, "AES");

          Cipher AESEncrypter = Cipher.getInstance( "AES/ECB/PKCS5Padding" );
          AESEncrypter.init( Cipher.ENCRYPT_MODE, AESpublicKey );
          //System.out.println("done");
          Cipher AESDecrypter = Cipher.getInstance( "AES/ECB/PKCS5Padding" );
          AESDecrypter.init( Cipher.DECRYPT_MODE, AESpublicKey );

          // Get the first client command
          String request = new String( getMessage( input ) );

          // All requests start with a verb.
          while ( ! request.equals( "quit" ) ) {
            String[] requests = request.split(" ");
            StringBuilder reply = new StringBuilder();
            if(requests[0].equals("query")){
              int result = 0; 
              String err = "";
              if(requests.length != 2){
                err = "Invalid Command!";
              }
              else{
                requests[1] = requests[1].toLowerCase();
                for(int i = 0; i < requests[1].length() && requests[1].length() <= WORD_MAX; i++){
                  int idx = (int)requests[1].charAt(i) - 'a'; 
                  if(idx < 0 || idx > WORD_MAX){
                    err = "Invalid Command!";
                    //System.out.println("Invalid Command!");
                    break;
                  }
                  result += SCORE[idx];
                }
              }
              if(result == 0 || err != "")
                reply.append("Invalid command!\n");
              else 
                reply.append(result + "\n");
            }

            else if(requests[0].equals("submit")){
              String err = "";
              int result = 0;
              String word = "";
              if(requests.length != 2){
                err = "Invalid Command!";
              }
              else{
                word = requests[1];
                requests[1] = requests[1].toLowerCase();
                for(int i = 0; i < requests[1].length() && requests[1].length() <= WORD_MAX; i++){
                  int idx = (int)requests[1].charAt(i) - 'a'; 
                  if(idx < 0 || idx > 26){
                    err = "Invalid Command!";
                    //System.out.println("Invalid Command!");
                    break;
                  }
                  result += SCORE[idx];
                }
              }
              if(result == 0 || err != "")
                reply.append("Invalid command!\n");
              else{
                synchronized(lock){
                  rec.score = result; 
                  rec.word = word;
                }
                //reply.append("");
              }     
            }
            else if(requests[0].equals("report")){
              synchronized(lock){     
                userList.sort((o1, o2)
                -> o1.score - o2.score);
                for(int i = 0; i < userList.size(); i++){
                  if(userList.get(i).score > 0){
                    reply.append(userList.get(i).name + "\t\t" + userList.get(i).word + "\t" + userList.get(i).score + "\n");
                  }
              }
                // System.out.println(userList.get(i).name);
                // System.out.println(userList.get(i).word);
                // System.out.println(userList.get(i).score);
              }
              
            }
            else{
              reply.append("Invalid command!\n");
            }
            putMessage( output, reply.toString().getBytes() );
            //if(request.equals(""))
            // Get the next command.
            request = new String( getMessage( input ) );
          }
        }
      } catch ( IOException e ) {
        System.out.println( "IO Error: " + e );
      } catch( GeneralSecurityException e ){
        System.err.println( "Encryption error: " + e );
      } finally {
        try {
          // Close the socket on the way out.
          sock.close();
        } catch ( Exception e ) {
        }
      }
    }
  }

  /** Port number used by the server */
  public static final int PORT_NUMBER = 28123;

  /** Maximum length of a username. */
  public static int NAME_MAX = 10;
  
  /** Maximum length of a word. */
  public static int WORD_MAX = 24;
  
  /** Record for an individual user. */
  private static class UserRec {
    // Name of this user.
    String name;

    //Word 
    String word; 

    //score
    int score; 

    // This user's public key.
    PublicKey publicKey;
  }

  /** scores of the letters in the scrabble game. */
  int SCORE[] = new int[]{1,3,3,2,1,4,2,4,1,8,5,1,3,1,1,3,10,1,1,1,1,4,4,8,4,10}; 

  /** List of all the user records. */
  private ArrayList< UserRec > userList = new ArrayList< UserRec >();

  // @Override
  // public int compareTo(UserRec compareuser) {
  //     int comparescore=((UserRec)compareuser).score;
  //     /* For Ascending order*/
  //     return this.score-comparescore;

  //     /* For Descending order do like this */
  //     //return comparescore-this.score;
  // }

  /** Read the list of all users and their public keys. */
  private void readUsers() throws Exception {
    Scanner input = new Scanner( new File( "passwd.txt" ) );
    while ( input.hasNext() ) {
      // Create a record for the next user.
      UserRec rec = new UserRec();
      rec.name = input.next();
      rec.word = ""; 
      rec.score = 0;
      // Get the key as a string of hex digits and turn it into a byte array.
      String base64Key = input.nextLine().trim();
      byte[] rawKey = Base64.getDecoder().decode( base64Key );
    
      // Make a key specification based on this key.
      X509EncodedKeySpec pubKeySpec = new X509EncodedKeySpec( rawKey );

      // Make an RSA key based on this specification
      KeyFactory keyFactory = KeyFactory.getInstance( "RSA" );
      rec.publicKey = keyFactory.generatePublic( pubKeySpec );

      // Add this user to the list of all users.
      userList.add( rec );
    }
  }

  /** Utility function to read a length then a byte array from the
      given stream.  TCP doesn't respect message boundaraies, but this
      is essientially a technique for marking the start and end of
      each message in the byte stream.  As a public, static method,
      this can also be used by the client. */
  public static byte[] getMessage( DataInputStream input ) throws IOException {
    int len = input.readInt();
    byte[] msg = new byte [ len ];
    input.readFully( msg );
    return msg;
  }

  /** Function analogous to the previous one, for sending messages. */
  public static void putMessage( DataOutputStream output, byte[] msg ) throws IOException {
    // Write the length of the given message, followed by its contents.
    output.writeInt( msg.length );
    output.write( msg, 0, msg.length );
    output.flush();
  }


}
