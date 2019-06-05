// Joe Gildner
// CSCI 447 Homework5
// 06/07/2019

import java.util.Random;
import java.util.Arrays;

public class DiskSim{

  private static final int DISKSIZE = 10000;
  private static final int RANDREQUESTS = 1000;

  private static int startLocation;
  private static boolean movingUp;
  private static int[] requests;

  public static void main(String[] args){

    switch (args.length){
      case 1:
        createRandomParams();
        break;
      case 3:
        startLocation = Integer.parseInt(args[0]);
        movingUp = args[1].equals("H");
        createRandomRequests(Integer.parseInt(args[2]));
        break;
      default:
        startLocation = Integer.parseInt(args[0]);
        movingUp = args[1].equals("H");
        parseRequests(Arrays.copyOfRange(args,2,args.length));
    }

    System.out.printf("Start: %d\n",startLocation);
    System.out.printf("Moving Up?: %B\n", movingUp);
    System.out.println("Requests: "+Arrays.toString(requests));

  }

  public static void createRandomParams(){
    Random rand = new Random();

    startLocation = rand.nextInt(DISKSIZE);
    movingUp = rand.nextBoolean();
    
    createRandomRequests(DISKSIZE);
  }

  public static void createRandomRequests(int reqCount){
    Random rand = new Random();
    requests = new int[reqCount];
    for(int i=0; i<reqCount; i++){
      requests[i] = rand.nextInt(reqCount);
    }

  }

  public static void parseRequests(String[] reqs){
    int reqCount = reqs.length;
    requests = new int[reqCount];
    for(int i=0; i<reqCount; i++){
      requests[i] = Integer.parseInt(reqs[i]);
    }
  }


}
