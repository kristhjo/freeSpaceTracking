#include <cxxabi.h>
#include <time.h>
#include <stdlib.h>
#include "cameragui.h"
#include "imageprocessing.h"

CameraGui::CameraGui(QObject *parent) :
    QObject (parent)
{
    this->m_CamInfo = datacontainers::CamInfo();

    this->newCamParameter = false;
    this->acquireImages = false;
    this->isPictureInProcess = false;
    this->m_CamThread = nullptr;
    this->m_imageContainer = nullptr;

    this->pm_systemList = nullptr;
    this->pm_pSystem = nullptr;
    this->m_sSystemID="";
    this->pm_interfaceList = nullptr;
    this->pm_pInterface = nullptr;
    this->m_sInterfaceID ="";
    this->pm_deviceList = nullptr;
    this->pm_pDevice = nullptr;
    this->m_sDeviceID="";
    this->pm_datastreamList = nullptr;
    this->pm_pDataStream = nullptr;
    this->m_sDataStreamID ="";
    this->pm_bufferList = nullptr;
    this->pm_pBuffer = nullptr;
    this->pm_imgProcessor = nullptr;
    this->pm_pImage = nullptr;
    this->pm_pTransformImage = nullptr;

}

CameraGui::~CameraGui()
{
    try{
        cv::destroyAllWindows();
        pm_pDevice->Close();
        pm_pInterface->Close();
        pm_pSystem->Close();
        BGAPI2::SystemList::ReleaseInstance();
    }
    catch (BGAPI2::Exceptions::IException& ex){
        std::cout << "ExceptionType:    " << ex.GetType() << '\n';
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << '\n';
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }
}


void CameraGui::Connect(std::stringstream &ss)
{
    //---------------------Load Image Processor--------------------------------------
    //---------------------        Start       --------------------------------------
    try{
        pm_imgProcessor = new BGAPI2::ImageProcessor();
        std::cout << "Version: " << pm_imgProcessor->GetVersion() << std::endl;
    }
    catch (BGAPI2::Exceptions::IException& ex){
        std::cout << "ExceptionType: " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function: " << ex.GetFunctionName() << std::endl;
    }
    //---------------------        End       --------------------------------------
    //---------------------Load Image Processor--------------------------------------

    //---------------------    Load System     --------------------------------------
    //---------------------        Start       --------------------------------------
    try{
        pm_systemList = SystemList::GetInstance();
        pm_systemList->Refresh();
        ss << "5.1.2 Detected systems: " << pm_systemList->size() << '\n';
    }
    catch (BGAPI2::Exceptions::IException& ex){
        ss << "ExceptionType: " << ex.GetType() << '\n';
        ss << "ErrorDescription: " << ex.GetErrorDescription() << '\n';
        ss << "in function: " << ex.GetFunctionName() << '\n';
        return;
    }


    for (SystemList::iterator sys = pm_systemList->begin(); sys != pm_systemList->end(); sys++){
        if(sys->second->IsOpen())
            sys->second->Close();

        sys->second->Open();
        m_sSystemID = sys->first;
        break;
    }


    if (m_sSystemID == ""){
        ss << "no system found" << '\n';
        return; // no system found
    }
    else{
        pm_pSystem = (*pm_systemList)[m_sSystemID];
    }

    //---------------------        End       --------------------------------------
    //---------------------    Load System   --------------------------------------


    //---------------------    Load Interface  --------------------------------------
    //---------------------        Start       --------------------------------------

    pm_interfaceList = pm_pSystem->GetInterfaces();
    pm_interfaceList->Refresh(100);
    ss << "5.1.4 Detected interfaces: " << pm_interfaceList->size() << '\n';
    //Open Interface
    for(InterfaceList::iterator ifc = pm_interfaceList->begin(); ifc != pm_interfaceList->end(); ifc++){
        ifc->second->Open();
        m_sInterfaceID = ifc->first;
        break;
    }

    if (m_sInterfaceID == ""){
        ss<<"no interface found \n";
        pm_pSystem->Close();
        BGAPI2::SystemList::ReleaseInstance();
        return; // no interface found
    }
    else{
        pm_pInterface = (*pm_interfaceList)[m_sInterfaceID];
    }
    //---------------------        End       --------------------------------------
    //---------------------  Load Interface  --------------------------------------


    //---------------------    Load Devices     --------------------------------------
    //---------------------        Start       --------------------------------------
    //defining devices
    pm_deviceList = pm_pInterface->GetDevices();
    pm_deviceList->Refresh(100);
    ss << "5.1.6 Detected devices: " << pm_deviceList->size() << '\n';
    std::string temp_sDeviceID;
    for (DeviceList::iterator dev = pm_deviceList->begin(); dev != pm_deviceList->end(); dev++){ //loops through the detected Baumer devices in the network. Connects to the one with ID identical to BaumerID in the CamInfo struct.
        try {

        dev->second->Open();
        temp_sDeviceID = dev->first;

        } catch (...) {
            continue;
        }

	if (this->m_CamInfo.BaumerID == "Baumer Bisamberg"){
            if (temp_sDeviceID == this->m_CamInfo.BaumerBisamberg){
                std::cout << "connecting to Baumer Bisamberg" << std::endl;
                m_sDeviceID = dev->first;
                break;
            }
        }
        else if (this->m_CamInfo.BaumerID == "Baumer IQOQI"){
            if (temp_sDeviceID == this->m_CamInfo.BaumerIQOQI){
                std::cout << "connecting to Baumer IQOQI" << std::endl;
                m_sDeviceID = dev->first;
                break;
            }
        }
    }

    if (m_sDeviceID == ""){
        ss << "no device found \n";
        return; // no device found
    }
    else{
        pm_pDevice = (*pm_deviceList)[m_sDeviceID];
    }

    this->m_CamInfo.DeviceNr = static_cast<std::string>(m_sDeviceID);
     //------------------  Set Camera Parameters -------------------------------------
     //---------------------        End         --------------------------------------


     //--------------------- Load DataStreams   --------------------------------------
     //---------------------        Start       --------------------------------------

    try{ //getting datastream
         //COUNTING AVAILABLE DATASTREAMS
         pm_datastreamList = pm_pDevice->GetDataStreams();
         pm_datastreamList->Refresh();
         ss << "5.1.8   Detected datastreams:     " << pm_datastreamList->size() << '\n';

         //DATASTREAM INFORMATION BEFORE OPENING
         for (DataStreamList::iterator dstIterator = pm_datastreamList->begin(); dstIterator != pm_datastreamList->end(); dstIterator++){
             ss << " DataStream ID:          " << dstIterator->first << '\n' << '\n';
         }


         //Datastream opening
         for(DataStreamList::iterator dst= pm_datastreamList->begin();dst != pm_datastreamList->end(); dst++){
             dst->second->Open();
             m_sDataStreamID = dst->first;
             break;
          }

         if (m_sDataStreamID == ""){
             ss << "no DataStream found \n";
             return; // no datastream found
         }
         else{
             ss << "DataStream Open \n";
             pm_pDataStream = (*pm_datastreamList)[m_sDataStreamID];
         }
     }
     catch (BGAPI2::Exceptions::IException& ex){
         ss << "ExceptionType:    " << ex.GetType() << '\n';
         ss << "ErrorDescription: " << ex.GetErrorDescription() << '\n';
         ss << "in function:      " << ex.GetFunctionName() << '\n';
         return;
     }

    //--------------------- Load DataStreams   --------------------------------------
    //---------------------        End         --------------------------------------

    ss << "Succesfully connected to camera \n";

    printCamInfo(ss);
    this->isCameraConnected->store(true, std::memory_order_release);
}


void CameraGui::Start(std::stringstream &ss)
{
    //---------------------    Load Buffer     --------------------------------------
    //---------------------        Start       --------------------------------------

    this->pm_bufferList = pm_pDataStream->GetBufferList();
    for(int i=0; i<10; i++){ // 4 buffers using internal buffers
        this->pm_pBuffer = new BGAPI2::Buffer();
        this->pm_bufferList->Add(pm_pBuffer);
    }
    ss << "5.1.10 Announced buffers: " << pm_bufferList->size() << '\n';

    //Allocate buffer for the datastream
    for (BufferList::iterator buf = this->pm_bufferList->begin(); buf != this->pm_bufferList->end(); buf++){
        buf->second->QueueBuffer();
    }
    ss << "5.1.11 Queued buffers: " << pm_bufferList->GetQueuedCount() << '\n';


    //---------------------    Load Buffer   --------------------------------------
    //---------------------        End       --------------------------------------

    //---------------------    Start Stream  --------------------------------------
    //---------------------        start     --------------------------------------
    //Start image acquisition

    this->pm_pDataStream->StartAcquisitionContinuous();
    this->pm_pDevice->GetRemoteNode("AcquisitionStart")->Execute();
    std::cout<< this->pm_pDevice->GetRemoteNode("AcquisitionFrameRateEnable")->GetBool() << std::endl;
    this->pm_pDevice->GetRemoteNode("AcquisitionFrameRateEnable")->SetBool(true);
    this->pm_pDevice->GetRemoteNode("AcquisitionFrameRate")->SetValue(1);
    this->isCameraRunning->store(true, std::memory_order_release);
    this->isPictureInProcess.store(true, std::memory_order_release);
    this->acquireImages.store(true, std::memory_order_release);
    this->m_CamThread = std::make_unique<std::thread>(&CameraGui::Run,this); //camthread handles image acquisition and display. This runs in the background until the camera is told to stop.

    //---------------------   Start Stream --------------------------------------
    //---------------------        End     --------------------------------------
}

void CameraGui::Run()
{
    //---------------------    Polling Picture Stream  --------------------------------------
    //---------------------            start           --------------------------------------

    this->updateCamParameters();
    BGAPI2::Buffer * pBufferFilled = nullptr;
        while (this->acquireImages.load(std::memory_order_acquire)) //change this to isStopping
        {
            pBufferFilled = this->pm_pDataStream->GetFilledBuffer(1000); //timeout 1000 msec
            if (this->newCamParameter.load(std::memory_order_acquire) == true){ //updates camera settings if there's new input from the main gui.
                this->updateCamParameters();
                this->newCamParameter.store(false, std::memory_order_release);
            }
            if(pBufferFilled == nullptr){
                std::cout << "Error: Buffer Timeout after 1000 msec" << std::endl;
            }
            else if(pBufferFilled->GetIsIncomplete() == true){
                std::cout << "Error: Image is incomplete" << std::endl;
                // queue buffer again
                pBufferFilled->QueueBuffer();
            }
            else{
                cv::Mat cvimg = cv::Mat(pBufferFilled->GetHeight(), pBufferFilled->GetWidth(), CV_8U, pBufferFilled->GetMemPtr()); //creates an opencv image matrix with the image in the buffer
                if (this->isCameraTracking->load(std::memory_order_acquire)){ // if tracking is activated, calculate centroids with either windowing or thresholding.
                    cv::Mat mask, croppedImg;
                    if (this->m_TrackingParameters.useWindowing.load(std::memory_order_acquire) == true){
                        imageprocessing::cropWindow(cvimg,croppedImg,this->m_TrackingParameters.WindowRadius);
                    }
                    else {
                        imageprocessing::cropThreshold(cvimg, croppedImg,this->m_TrackingParameters.TrackingThresh);
                    }
                    cv::Point centroid = imageprocessing::findCentroid(croppedImg);


                    this->currentCentroid = centroid;
                    if (this->isHexapodStabilizing->load(std::memory_order_acquire)){ //if hexapodstabilization is activated, share the centroids with hexapodgui.
                        this->centroidContainer->currentCentroid = centroid;
                        this->centroidContainer->addCentroid(centroid);
                        //this->centroidContainer->newCentroid.store(true, std::memory_order_release);
                    }
                    if (this->isHedyLamarrStabilizing->load(std::memory_order_acquire)){ //if hexapodstabilization is activated, share the centroids with hexapodgui.
                        this->centroidContainerHedy->currentCentroid = centroid;
                        this->centroidContainerHedy->addCentroid(centroid);
                        //this->centroidContainer->newCentroid.store(true, std::memory_order_release);
                    }
                    cv::circle(cvimg,centroid,5,cv::Scalar(128,0,0),-1); //creates circle on img following the centroid.
                    cv::hconcat(cvimg, croppedImg, croppedImg);
                    cv::Point pText(20,20);
                    std::string centroidText = "x: " + std::to_string(centroid.x) + "   " + "y: " + std::to_string(centroid.y);
                    cv::putText(croppedImg, centroidText, pText,cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(128,128,256));
                    cv::imshow("tracking", croppedImg);

                }
                else if (this->isMeasuringSeeing->load(std::memory_order_acquire) == true){ //if a seeing measurement is activated, share image with seeinggui.
                    if (this->m_imageContainer->imgCounter == 1){
                        this->m_imageContainer->startTime = time(nullptr);
                    }
                    else if (this->m_imageContainer->imgCounter == this->m_imageContainer->sampleSize){
                        this->m_imageContainer->endTime = time(nullptr);
                    }
                    if (this->m_imageContainer->imgCounter < this->m_imageContainer->sampleSize ){
                        this->m_imageContainer->addImg(cvimg.clone());
                        this->m_imageContainer->imgCounter++;
                    }
                    else { //if the imgCounter reaches the sample size, wait for seeinggui to empty imgQueue and reset the imgCounter.
                        while(this->m_imageContainer->imgQueue.size()!=0){
                            std::this_thread::sleep_for(std::chrono::seconds(1));
                        }
                        this->m_imageContainer->imgCounter = 1;

                    }
                    cv::imshow("Seeing measurement", cvimg);
                }
                else{ //if no tracking, of seeing measurements are active, simply display the image.
                    cv::imshow("no tracking", cvimg);
                }
                cvimg.release();
                pBufferFilled->QueueBuffer();
            }
        }
    std::cout << "end of picture polling" << std::endl;
    this->isPictureInProcess.store(false,std::memory_order_release);

    //---------------------    Polling Picture Stream  --------------------------------------
    //---------------------            End             --------------------------------------

}

void CameraGui::updateCamParameters(){
    if (this->pm_pDevice->GetRemoteNode("ExposureTime")->IsWriteable() == true){
        this->pm_pDevice->GetRemoteNode("ExposureTime")->SetDouble(this->m_CamInfo.ExposureActual);
    }

    if (this->pm_pDevice->GetRemoteNode("Gain")->IsWriteable() == true){
        this->pm_pDevice->GetRemoteNode("Gain")->SetInt(this->m_CamInfo.GainActual);
    }

    if (this->pm_pDevice->GetRemoteNode("Width")->IsWriteable() == true){
        try {
            this->pm_pDevice->GetRemoteNode("Width")->SetInt(this->m_CamInfo.FrameWidth);
        } catch(BGAPI2::Exceptions::IException& ex)
        {
            std::cout << "ExceptionType:    " << ex.GetType() << '\n';
            std::cout << "ErrorDescription: " << ex.GetErrorDescription() << '\n';
            std::cout << "in function:      " << ex.GetFunctionName() << '\n';
        }
    }

    if (this->pm_pDevice->GetRemoteNode("Height")->IsWriteable() == true){
        try {
            this->pm_pDevice->GetRemoteNode("Height")->SetInt(this->m_CamInfo.FrameHeight);
        }
        catch(BGAPI2::Exceptions::IException& ex){
            std::cout << "ExceptionType:    " << ex.GetType() << '\n';
            std::cout << "ErrorDescription: " << ex.GetErrorDescription() << '\n';
            std::cout << "in function:      " << ex.GetFunctionName() << '\n';
        }
    }
    if (this->pm_pDevice->GetRemoteNode("OffsetX")->IsWriteable() == true){
        try {
            this->pm_pDevice->GetRemoteNode("OffsetX")->SetInt(this->m_CamInfo.OffsetX);
        }
        catch(BGAPI2::Exceptions::IException& ex){
            std::cout << "ExceptionType:    " << ex.GetType() << '\n';
            std::cout << "ErrorDescription: " << ex.GetErrorDescription() << '\n';
            std::cout << "in function:      " << ex.GetFunctionName() << '\n';
        }
    }
    if (this->pm_pDevice->GetRemoteNode("OffsetY")->IsWriteable() == true){
        try {
             this->pm_pDevice->GetRemoteNode("OffsetY")->SetInt(this->m_CamInfo.OffsetY);
        }
        catch(BGAPI2::Exceptions::IException& ex){
            std::cout << "ExceptionType:    " << ex.GetType() << '\n';
            std::cout << "ErrorDescription: " << ex.GetErrorDescription() << '\n';
            std::cout << "in function:      " << ex.GetFunctionName() << '\n';
        }
    }
}

void CameraGui::Stop(std::stringstream &ss)
{
    this->acquireImages.store(false, std::memory_order_release);
    while (isPictureInProcess.load(std::memory_order_acquire)) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        //wait until the image acquisition loop has terminated.
        std::cout << "Waiting for image acquisition to end" << std::endl;
    }
     if(m_CamThread->joinable())
    {
        std::cout << "Closing thread"<< std::endl;
        m_CamThread->join();
    }
    else
    {
       ss << "Serios thread error occured\n";
       return;
    }

    try
    {   //stop acquisition and release Buffer
        pm_pDevice->GetRemoteNode("AcquisitionStop")->Execute();
        pm_pDataStream->StopAcquisition();

        std::cout << "5.1.12   " << pm_pDevice->GetModel() << " stopped " << std::endl;
        pm_bufferList->DiscardAllBuffers();
        while( pm_bufferList->size() > 0)
        {
            pm_pBuffer = pm_bufferList->begin()->second;
            pm_bufferList->RevokeBuffer(pm_pBuffer);
            delete pm_pBuffer;
        }
        ss << "         buffers after revoke:    " << pm_bufferList->size() << '\n';
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        ss << "ExceptionType:    " << ex.GetType() << '\n';
        ss << "ErrorDescription: " << ex.GetErrorDescription() << '\n';
        ss << "in function:      " << ex.GetFunctionName() << '\n';
        return;
    }
    ss << "Camera stopped succesfully \n";
    this->isCameraRunning->store(false, std::memory_order_release);
}

void CameraGui::Disconnect(std::stringstream &ss)
{
    try{ //close the interface to the Baumer camera
        this->pm_pDataStream->Close();
        this->pm_pDevice->Close();
        this->pm_pInterface->Close();
        this->pm_pSystem->Close();
        BGAPI2::SystemList::ReleaseInstance();
    }
    catch (BGAPI2::Exceptions::IException& ex){
        ss << "ExceptionType:    " << ex.GetType() << '\n';
        ss << "ErrorDescription: " << ex.GetErrorDescription() << '\n';
        ss << "in function:      " << ex.GetFunctionName() << '\n';
        return;
    }
    cv::destroyAllWindows();
    ss << "Device disconnected \n";
    this->isCameraTracking->store(false, std::memory_order_release);
    this->isCameraRunning->store(false, std::memory_order_release);
    this->isCameraConnected->store(false, std::memory_order_release);
}

void CameraGui::printCamInfo(std::stringstream &ss) {
    ss << "Camera Parameters: \n";
    ss << "Gain: " << this->m_CamInfo.GainActual << "\n";
    ss << "Exposure time: " << this->m_CamInfo.ExposureActual<< "\n";
    ss << "Height: " << this->m_CamInfo.FrameHeight << "\n";
    ss << "Width: " << this->m_CamInfo.FrameWidth << "\n";
    ss << "XOffset: " << this->m_CamInfo.OffsetX << "\n";
    ss << "YOffset: " << this->m_CamInfo.OffsetY << "\n";
}
