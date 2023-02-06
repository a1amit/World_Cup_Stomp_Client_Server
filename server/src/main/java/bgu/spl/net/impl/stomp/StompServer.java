package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Server;

public class StompServer {


    public static void main(String[] args) {
        final String  TPC = "tpc";
        final String REACTOR = "reactor";


       int port = 7777;
        // int port = Integer.parseInt(args[0]);
        // String serverType = args[1];
       String serverType = TPC;
        ConnectionsImpl<String> connections = new ConnectionsImpl<>();

        /**
         * mvn exec:java -Dexec.mainClass="bgu.spl.net.impl.stomp.StompServer" -Dexec.args="7777 tpc"
         * mvn exec:java -Dexec.mainClass="bgu.spl.net.impl.stomp.StompServer" -Dexec.args="7777 reactor"
         */

        // choose the server type and start the server
        if (serverType.equals(TPC)) {
            Server.threadPerClient(
                    port, //port
                    StompProtocol::new, //currently shows problems because we didn't change the interface
                    StompEncoderDecoder::new //message encoder decoder factory
            ).serve(connections);
        } else if (serverType.equals(REACTOR)) {
            Server.reactor(
                    Runtime.getRuntime().availableProcessors(),
                    port, //port
                    StompProtocol::new, //currently shows problems becasue we didn't change the interface
                    StompEncoderDecoder::new //message encoder decoder factory
            ).serve(connections);
        }
    }
}
