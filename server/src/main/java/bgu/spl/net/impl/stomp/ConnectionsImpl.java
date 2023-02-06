package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;

import java.io.IOException;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

public class ConnectionsImpl<T> implements Connections<T> {

    /**
     * represents the active connections by ID
     */
    private ConcurrentHashMap<Integer, ConnectionHandler<T>> activeConnections;

    /**
     * represents the users that are subscribed to the channel
     * map<Channel : Set<ConnectionIds>>
     * used when we want to send a message to all the clients subbed to this channel
     */
    private ConcurrentHashMap<String, Set<Integer>> subscriptionsByChannelName;

    /**
     * maps between connectionId to the id of the user in the channel
     * map<connectionId : map<Id,topic>
     */
    private ConcurrentHashMap<Integer, ConcurrentHashMap<Integer, String>> userSubscribedChannelsByConnectionId;


    /**
     * the users HashMap(acts as volatile dataBase).
     * users<login : passcode>
     */
    private ConcurrentHashMap<String, String> users;


    /**
     * keeps track on the logged-in users
     */
    private ConcurrentLinkedQueue<String> loggedInUsers;

    /**
     * maps between a connectionId to the username
     */
    private ConcurrentHashMap<Integer, String> connectionIdToUsername;


    /**
     * constructor
     */
    public ConnectionsImpl() {
        activeConnections = new ConcurrentHashMap<>();
        subscriptionsByChannelName = new ConcurrentHashMap<>();
        users = new ConcurrentHashMap<>();
        loggedInUsers = new ConcurrentLinkedQueue<>();
        userSubscribedChannelsByConnectionId = new ConcurrentHashMap<>();
        connectionIdToUsername = new ConcurrentHashMap<>();
    }


    /**
     * sends a message to client represented by the given connectionId.
     *
     * @param connectionId =  the id of the client
     * @param msg          = the message
     * @return true if the msg was sent and false otherwise
     */
    @Override
    public boolean send(int connectionId, T msg) {
        if (activeConnections.containsKey(connectionId)) {
            ConnectionHandler<T> handler = activeConnections.get(connectionId);
            handler.send(msg);
            return true;
        }
        return false;
    }


    /**
     * disconnects the client by removing him from the connections map
     *
     * @param connectionId = the id of the client connected
     */
    @Override
    public void disconnect(int connectionId) {
        ConnectionHandler<T> connectionHandler = activeConnections.remove(connectionId);
        if (connectionHandler == null) {
            // in case the user isn't logged-in we do nothing
            return;
        }
        // try {
            String username = connectionIdToUsername.get(connectionId); // gets the username based on ID
            loggedInUsers.remove(username); // removes the user from logged-in users
            connectionIdToUsername.remove(connectionId); // removes the user from the map<connectionId:username>
            activeConnections.remove(connectionId); // removes the connection handler from the active connections
            // connectionHandler.close();
        // } catch (IOException ignored) {
        //     System.out.println("failed to disconnect");
        // }
    }


    /**
     * adds a new user to the subscription channel
     *
     * @param topic
     * @param connectionId
     */
    public void addSubscription(String topic, int connectionId, int Id) {
        Set<Integer> connectionIds = subscriptionsByChannelName.computeIfAbsent(topic, k -> new HashSet<>());
        connectionIds.add(connectionId); // adds the user to topic map that contains all connection ids subscribed to channel
        ConcurrentHashMap<Integer, String> connectionIdToChannel = new ConcurrentHashMap<>(); // used to save ID per user
        connectionIdToChannel.put(Id, topic);  // map<Id : channel>
        userSubscribedChannelsByConnectionId.put(connectionId, connectionIdToChannel); // adds the channel to clients channel by id map;
    }

    @Override
    public void addConnection(int connectionId, ConnectionHandler<T> connectionHandler) {
        activeConnections.put(connectionId, connectionHandler);
    }


    /**
     * removes a user from channel
     *
     * @param connectionID
     * @param id
     * @return
     */
    public String removeSubscription(int connectionID, int id) {
        //get the clients IDS of subscribed channels
        ConcurrentHashMap<Integer, String> IdByChannel = userSubscribedChannelsByConnectionId.get(connectionID);
        // get the channel name from the clients id
        String channel = IdByChannel.get(id);
        if (channel == null) {
            //in case the user is not even subscribed to this channel
            return null;
        } else {
            Set<Integer> idsSubscribedToChannel = subscriptionsByChannelName.get(channel);
            //removes the id from the main channel subscribers map
            idsSubscribedToChannel.remove(id);
            // removes the channel from the users id map
            IdByChannel.remove(id);
            return channel;
        }
    }

    /**
     * adds a user to the users Map
     *
     * @param username
     * @param passcode
     */
    public void addUser(String username, String passcode, int ConnectionId) {
        if (!users.containsKey(username)) {
            users.put(username, passcode); //adds the new user to the "Database"
            loggedInUsers.add(username); // adds this user to the loggedIn users
            connectionIdToUsername.put(ConnectionId, username); // maps between the connectionId to the username
//            registeredConnectionIds.add(ConnectionId); // adds the connectionId to the registered users
        }
    }

    /**
     * checks whether the user is already logged in by username or not
     *
     * @param username
     * @return
     */
    public boolean isUserLoggedIn(String username) {
        return loggedInUsers.contains(username);
    }

    /**
     * checks whether the user is already logged-in by connectionId
     *
     * @param connectionId
     * @return
     */
    public boolean isUserLoggedIn(int connectionId) {
        return (connectionIdToUsername.get(connectionId) != null);
    }

    /**
     * checks if the user entered a valid passcode
     *
     * @param username
     * @param passcode
     * @return
     */
    public boolean isValidLogin(String username, String passcode, int connectionId) {
        if (!users.containsKey(username)) { // if the user does not exist
            return false;
        }

        String storedPassCode = users.get(username);// retrieves users passcode from the hash map
        if (storedPassCode != null && storedPassCode.equals(passcode)) {
            loggedInUsers.add(username);
            connectionIdToUsername.put(connectionId, username);
            return true;
        }
        return false;
    }

    public boolean userExists(String username) {
        return users.containsKey(username);
    }

    /**
     * checks if a user is subscribed to the channel by connectionID
     *
     * @param connectionId
     * @param destination
     * @return
     */
    @Override
    public boolean isSubscribed(int connectionId, String destination) {
        Set<Integer> subscribers = subscriptionsByChannelName.get(destination);
        if (subscribers == null) {
            //in case the channel wasn't created yet
            return false;
        }
        return subscribers.contains(connectionId);
    }

    public Integer getSubscriptionId(int connectionId, String destination) {
        //get all users ids
        ConcurrentHashMap<Integer, String> idChannelMap = userSubscribedChannelsByConnectionId.get(connectionId);

        for (Map.Entry<Integer, String> entry : idChannelMap.entrySet()) {
            if (entry.getValue().equals(destination)) {
                // found the ChannelId of the user in the map
                return entry.getKey();
                // do something with the key here
            }
        }
        // if key wasn't found
        return null;
    }

    public Set<Integer> getSubscriptionsByChannelName(String channel) {
        Set<Integer> currentChannel = subscriptionsByChannelName.get(channel);
        return currentChannel;
    }


}
