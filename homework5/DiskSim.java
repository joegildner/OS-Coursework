// Joe Gildner
// CSCI 447 Homework5
// 06/07/2019

import java.util.Random;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Collections;

public class DiskSim{

  private static final int DISKSIZE = 10000;
  private static final int RANDREQUESTS = 1000;
  private static final String METHODS[] = {"FCFS", "SSTF", "SCAN", "C-SCAN", "LOOK", "C-LOOK"};
  private static final int FCFS = 0;
  private static final int SSTF = 1;
  private static final int SCAN = 2;
  private static final int CSCAN = 3;
  private static final int LOOK = 4;
  private static final int CLOOK = 5;


  private static int startLocation;
  private static boolean MOVING_UP;
  private static ArrayList<Integer> requests;

  private static LinkedList<Integer>[] histories = new LinkedList[6];
  private static int[] dirChanges = new int[6];
  private static int[] cylinderCounts = new int[6];


  public static void main(String[] args){

    switch (args.length){
      case 1:
        createRandomParams();
        break;
      case 3:
        startLocation = Integer.parseInt(args[0]);
        MOVING_UP = args[1].equals("H");
        createRandomRequests(Integer.parseInt(args[2].substring(1)));
        break;
      default:
        startLocation = Integer.parseInt(args[0]);
        MOVING_UP = args[1].equals("H");
        parseRequests(Arrays.copyOfRange(args,2,args.length));
    }

    FCFS();
    SSTF();
    SCAN();
    CSCAN();
    LOOK();
    CLOOK();

    printResults();
  }

  //Create random paramters based on input values
  public static void createRandomParams(){
    Random rand = new Random();

    startLocation = rand.nextInt(DISKSIZE);
    MOVING_UP = rand.nextBoolean();
    
    createRandomRequests(1000);
  }

  public static void createRandomRequests(int reqCount){
    Random rand = new Random();
    requests = new ArrayList<Integer>();

    for(int i=0; i<reqCount; i++){
      requests.add(rand.nextInt(DISKSIZE));
    }

  }

  public static void parseRequests(String[] reqs){
    int reqCount = reqs.length;
    requests = new ArrayList<Integer>();

    for(int i=0; i<reqCount; i++){
      requests.add(Integer.parseInt(reqs[i]));
    }

  }

  //Simulate the FCFS disk scheduling algorithm with the input parameters
  public static void FCFS(){
    histories[FCFS] = new LinkedList<Integer>();
    ArrayList<Integer> reqCopy = getReqCopy();
    int reversals = 0;
    int cylinders = 1;
    boolean up = MOVING_UP;

    int lastReq = startLocation;

    for(int nextReq : requests){

      histories[FCFS].add(nextReq);

      if((up && nextReq < lastReq) || (!up && nextReq > lastReq)){
        reversals++;
        up = !up;
      }

      cylinders += Math.abs(nextReq-lastReq);

      lastReq = nextReq;

    }

    dirChanges[FCFS] = reversals;
    cylinderCounts[FCFS] = cylinders;

  }

  ////Simulate the SSTF disk scheduling algorithm with the input parameters
  public static void SSTF(){
    histories[SSTF] = new LinkedList<Integer>();
    ArrayList<Integer> reqCopy = getReqCopy();
    int reversals = 0;
    int cylinders = 1;
    boolean up = MOVING_UP;

    int lastReq = startLocation;

    for(int req : requests){
      int nextReq = findShortestSeek(lastReq, reqCopy);
      histories[SSTF].add(nextReq);

      if((up && (nextReq < lastReq)) || (!up && (nextReq > lastReq))){
        reversals++;
        up = !up;
      }

      cylinders += Math.abs(nextReq-lastReq);

      lastReq = nextReq;

    }

    dirChanges[SSTF] = reversals;
    cylinderCounts[SSTF] = cylinders;

  }

  //Finds the next shortest seek for the SSTF algorithm
  public static int findShortestSeek(int lastReq, ArrayList<Integer> nextReqs){
    int sseek = Integer.MAX_VALUE;
    int sseekIndex = 0;

    int i=0;
    for(Integer req : nextReqs){
      if(Math.abs(lastReq-req) < sseek){
        sseekIndex = i;
        sseek = Math.abs(lastReq-req);
      } 
      i++;
    }

    return nextReqs.remove(sseekIndex);
  }

  public static ArrayList<Integer> getReqCopy(){
    ArrayList<Integer> reqCopy = new ArrayList<Integer>();

    for(Integer req : requests){
      reqCopy.add(req);
    }

    return reqCopy;
  }

  

  //Simulate the CSCAN disk scheduling algorithm with the input parameters
  public static void CSCAN(){
    boolean movingUp = MOVING_UP;
    histories[CSCAN] = new LinkedList<Integer>();
    ArrayList<Integer> reqCopy = getReqCopy();
    Collections.sort(reqCopy);
    cylinderCounts[CSCAN] = 1;

    boolean highEndClose = DISKSIZE - startLocation <= startLocation;
    int startIndex = -1;

    for(int i=0; i<reqCopy.size(); i++){
      if(highEndClose){
        if(!movingUp){
          movingUp = true;
          dirChanges[CSCAN]++;
        }
        if(reqCopy.get(i) >= startLocation){
          startIndex = i;
          break;
        }
      }else{
        if(movingUp){
          movingUp = false;
          dirChanges[CSCAN]++;
        }
        if(reqCopy.get(i) > startLocation){
          startIndex = i-1;
          break;
        }
      }
    }

    if(startIndex < 0){
      System.err.println("Ya done messed up");
    }

    int i = startIndex;
    if(highEndClose){

      while(i<reqCopy.size()){
        histories[CSCAN].add(reqCopy.get(i));
        i++;
      }

      dirChanges[CSCAN]++;
      cylinderCounts[CSCAN] += DISKSIZE-1 - startLocation;

      i = 0;
      dirChanges[CSCAN]++;
      cylinderCounts[CSCAN] += DISKSIZE-1;

      while(i < startIndex){
        histories[CSCAN].add(reqCopy.get(i));
        i++;
      }

      cylinderCounts[CSCAN] += reqCopy.get(startIndex-1);

    }else{
      while(i>=0){
        histories[CSCAN].add(reqCopy.get(i));
        i--;
      }

      dirChanges[CSCAN]++;
      cylinderCounts[CSCAN] += startLocation;

      i = reqCopy.size()-1;

      dirChanges[CSCAN]++;
      cylinderCounts[CSCAN] += DISKSIZE-1;

      while(i>startIndex){
        histories[CSCAN].add(reqCopy.get(i));
        i--;
      }

      cylinderCounts[CSCAN] += DISKSIZE -1 - reqCopy.get(startIndex+1);

    }
  }

  //Simulate the SCAN disk scheduling algorithm with the input parameters
  public static void SCAN(){
    boolean movingUp = MOVING_UP;
    histories[SCAN] = new LinkedList<Integer>();
    ArrayList<Integer> reqCopy = getReqCopy();
    Collections.sort(reqCopy);
    cylinderCounts[SCAN] = 1;

    boolean highEndClose = DISKSIZE - startLocation <= startLocation;
    int startIndex = -1;

    for(int i=0; i<reqCopy.size(); i++){
      if(highEndClose){
        if(!movingUp){
          movingUp = true;
          dirChanges[SCAN]++;
        }
        if(reqCopy.get(i) >= startLocation){
          startIndex = i;
          break;
        }
      }else{
        if(movingUp){
          movingUp = false;
          dirChanges[SCAN]++;
        }
        if(reqCopy.get(i) > startLocation){
          startIndex = i-1;
          break;
        }
      }
    }

    if(startIndex < 0){
      System.err.println("Ya done messed up");
    }

    int i = startIndex;
    if(highEndClose){

      while(i<reqCopy.size()){
        histories[SCAN].add(reqCopy.get(i));
        i++;
      }

      dirChanges[SCAN]++;
      cylinderCounts[SCAN] += DISKSIZE-1 - startLocation;

      i = startIndex-1;
      while(i>=0){
        histories[SCAN].add(reqCopy.get(i));
        i--;
      }

      cylinderCounts[SCAN] += DISKSIZE-1 - reqCopy.get(0);

    }else{
      while(i>=0){
        histories[SCAN].add(reqCopy.get(i));
        i--;
      }

      dirChanges[SCAN]++;
      cylinderCounts[SCAN] += startLocation - 1;

      i = startIndex+1;
      while(i<reqCopy.size()){
        histories[SCAN].add(reqCopy.get(i));
        i++;
      }

      cylinderCounts[SCAN] += reqCopy.get(reqCopy.size()-1);

    }
  }

  //Simulate the CLOOK disk scheduling algorithm with the input parameters
  public static void CLOOK(){
    boolean movingUp = MOVING_UP;
    histories[CLOOK] = new LinkedList<Integer>();
    ArrayList<Integer> reqCopy = getReqCopy();
    Collections.sort(reqCopy);
    cylinderCounts[CLOOK] =1;

    boolean highEndClose = DISKSIZE - startLocation <= startLocation;
    int startIndex = -1;

    for(int i=0; i<reqCopy.size(); i++){
      if(highEndClose){
        if(!movingUp){
          movingUp = true;
          dirChanges[CLOOK]++;
        }
        if(reqCopy.get(i) >= startLocation){
          startIndex = i;
          break;
        }
      }else{
        if(movingUp){
          movingUp = false;
          dirChanges[CLOOK]++;
        }
        if(reqCopy.get(i) > startLocation){
          startIndex = i-1;
          break;
        }
      }
    }

    if(startIndex < 0){
      System.err.println("Ya done messed up");
    }

    int i = startIndex;
    if(highEndClose){

      while(i<reqCopy.size()){
        histories[CLOOK].add(reqCopy.get(i));
        i++;
      }

      dirChanges[CLOOK]++;
      cylinderCounts[CLOOK] += reqCopy.get(reqCopy.size()-1) - startLocation;

      i = 0;
      dirChanges[CLOOK]++;
      cylinderCounts[CLOOK] += reqCopy.get(reqCopy.size()-1) - reqCopy.get(0);

      while(i < startIndex){
        histories[CLOOK].add(reqCopy.get(i));
        i++;
      }

      cylinderCounts[CLOOK] += reqCopy.get(startIndex-1) - reqCopy.get(0);

    }else{
      while(i>=0){
        histories[CLOOK].add(reqCopy.get(i));
        i--;
      }

      dirChanges[CLOOK]++;
      cylinderCounts[CLOOK] += startLocation - 1;

      i = reqCopy.size()-1;

      dirChanges[CLOOK]++;
      cylinderCounts[CLOOK] += reqCopy.get(reqCopy.size()-1) - reqCopy.get(0);

      while(i>startIndex){
        histories[CLOOK].add(reqCopy.get(i));
        i--;
      }

      cylinderCounts[CLOOK] += reqCopy.get(reqCopy.size()-1) - reqCopy.get(startIndex+1);

    }
  }

  //Simulate the LOOK disk scheduling algorithm with the input parameters
  public static void LOOK(){
    boolean movingUp = MOVING_UP;
    histories[LOOK] = new LinkedList<Integer>();
    ArrayList<Integer> reqCopy = getReqCopy();
    Collections.sort(reqCopy);
    cylinderCounts[LOOK] = 1;

    boolean highEndClose = (DISKSIZE - startLocation) <= startLocation;
    int startIndex = -1;

    for(int i=0; i<reqCopy.size(); i++){
      if(highEndClose){
        if(!movingUp){
          movingUp = true;
          dirChanges[LOOK]++;
        }
        if(reqCopy.get(i) >= startLocation){
          startIndex = i;
          break;
        }
      }else{
        if(movingUp){
          movingUp = false;
          dirChanges[LOOK]++;
          System.out.println("MOVING DOWN NOW");
        }
        if(reqCopy.get(i) > startLocation){
          startIndex = i-1;
          break;
        }
      }
    }

    if(startIndex < 0){
      System.err.println("Ya done messed up");
    }

    int i = startIndex;
    if(highEndClose){

      while(i<reqCopy.size()){
        histories[LOOK].add(reqCopy.get(i));
        i++;
      }

      dirChanges[LOOK]++;
      cylinderCounts[LOOK] += reqCopy.get(reqCopy.size()-1) - startLocation;

      i = startIndex-1;
      while(i>=0){
        histories[LOOK].add(reqCopy.get(i));
        i--;
      }

      cylinderCounts[LOOK] += reqCopy.get(reqCopy.size()-1) - reqCopy.get(0);

    }else{
      while(i>=0){
        histories[LOOK].add(reqCopy.get(i));
        i--;
      }

      dirChanges[LOOK]++;
      cylinderCounts[LOOK] += startLocation - reqCopy.get(0);

      i = startIndex+1;
      while(i<reqCopy.size()){
        histories[LOOK].add(reqCopy.get(i));
        i++;
      }

      cylinderCounts[LOOK] += reqCopy.get(reqCopy.size()-1) - reqCopy.get(0);

    }
  }

  // ---------- Print methods to display result stats --------- //
  public static void printResults(){
    System.out.println("== Service history ==");

    printHistories();

    System.out.println("== Service stats ==");

    printStats();
  }


  public static void printHistories(){
    int i=0;
    for(LinkedList<Integer> thisList : histories){
      System.out.print(METHODS[i]);
      for(Integer thisInt : thisList){
        System.out.printf(" %d", thisInt);
      }
      System.out.println();
      i++;
    }
  }

  public static void printStats(){

    for(int i=0; i<METHODS.length; i++){
      System.out.printf("%s %d %d\n", METHODS[i], dirChanges[i], cylinderCounts[i]);
    }

  }

}
