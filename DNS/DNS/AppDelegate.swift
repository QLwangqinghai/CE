//
//  AppDelegate.swift
//  DNS
//
//  Created by vector on 2019/8/7.
//  Copyright © 2019 angfung. All rights reserved.
//

import Cocoa



@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

    @IBOutlet weak var window: NSWindow!
    
    let buffer: UnsafeMutableRawPointer = UnsafeMutableRawPointer.allocate(byteCount: 2048, alignment: 1)

    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application
        buffer.initializeMemory(as: UInt8.self, repeating: 0, count: 2048)
        // Encoding a message
        let request = Message(
            type: .query,
//            questions: [Question(name: "aliyun.com", type: .pointer)]
            questions: [Question(name: "www.haoqi100.com", type: .pointer)]

        )
        
        let requestData = try! request.serialize()
        
        let sockfd = socket(AF_INET, SOCK_DGRAM, 0)
        if sockfd < 0 {
            print("socket error")
            abort()
        }
        
        var serveraddr: sockaddr_in = sockaddr_in()
        serveraddr.sin_family = sa_family_t(AF_INET)
        serveraddr.sin_port = UInt16(53).bigEndian//htonl(53)
        let str = "223.5.5.5"
        var strData = str.data(using: String.Encoding.utf8)!
        strData.append(0)
        let nd = (strData as NSData)
        let b = nd.bytes
        
//        let code = inet_aton(b.assumingMemoryBound(to: Int8.self), &serveraddr.sin_addr)
        let code = inet_aton(haddr, &serveraddr.sin_addr)

        if code == 0 {
            print("inet_aton error")
            printLastError()
            abort()
        }
        
        let _ = nd
        
        DispatchQueue.global().async {
            
            let code = recv(sockfd, self.buffer, 2048, 0)
            if code < 0 {
                print("recv error")
                abort()
            } else {
//                buffer.bind
//                Data.init(buffer: UnsafeBufferPointer<SourceType>)
//                self.buffer.bindMemory(to: <#T##T.Type#>, capacity: <#T##Int#>)
                
                let data = Data(bytes: self.buffer, count: code)
                if let str = String.init(data: data, encoding: .utf8) {
                    print("recved: \(str)")
                } else {
                    print("recved2: \(data)")
                }
                let responseData = data
                let response = try? Message.init(deserialize: responseData)
                print(response?.answers.first)

                
                print(response?.answers.first)
            }
        }
        
        let c = insendto(sockfd, (requestData as NSData).bytes, requestData.count, 0, &serveraddr, socklen_t(MemoryLayout<sockaddr_in>.size))
        

//        59     unsigned int ip = get_ip_by_name(argv[1]);
//        60     if(ip != 0){
//            61         serveraddr.sin_addr.s_addr = ip;
//            62     }
//        63     else {
//            64         inet_pton(AF_INET, argv[1], &serveraddr.sin_addr.s_addr);
//            65     }
//        66
//        67     char buffer[1024] = "hello world";
//        68     /** 向服务器端发送数据报文 */
//        69     if(sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0){
//            70         perror("sendto error");
//            71         exit(1);
//            72     }
//        73     else{
//            74         /** 接受服务器端发送的数据报文 */
//            75         memset(buffer, 0, sizeof(buffer));
//            76         if(recv(sockfd, buffer, sizeof(buffer), 0) < 0){
//                77             perror("recv error");
//                78             exit(1);
//                79         }
//            80         else{
//                81             printf("%s", buffer);
//                82         }
//            83     }
//        84
//        85     close(sockfd);
//        86
//
        
        // Not shown here: send to DNS server over UDP, receive reply.
        
        // Decoding a message

    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }


}

