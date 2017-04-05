#pragma once

#include "Scenario2_GetRawData.g.h"
#include "MainPage.xaml.h"
#include "SimpleLogger.h"
#include "FrameRenderer.h"
#include <wrl.h>
#include <wrl/client.h>

namespace SDKTemplate
{
	// This structure stores information related to a frame source.
	struct FrameSourceState2
	{
		bool enabled = false; // Whether or not this source is enabled for rendering.

		Windows::Media::Capture::Frames::MediaFrameSourceInfo^ sourceInfo = nullptr; // The source info associated with this source.
		Windows::Media::Capture::Frames::MediaFrameReference^ latestFrame = nullptr;  // The latest frame from this source.
		Windows::Media::Capture::Frames::MediaFrameReader^ reader = nullptr; // The reader we are using to read this source.

		Windows::Foundation::EventRegistrationToken frameArrivedEventToken;
	};

	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Scenario2_GetRawData sealed
	{
	public:
		Scenario2_GetRawData();

		void setBufferGrid(int gridIndex);

	protected:
		/// <summary>
		/// Called when user navigates to this Scenario.
		/// Immediately start streaming from first available source group.
		/// </summary>
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

		/// <summary>
		/// Called when user navigates away from this Scenario.
		/// Stops streaming and disposes of all objects.
		/// </summary>
		virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

	private:
		void NextButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void captureButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void DisplayImage();

	private: // Private methods
			 /// <summary>
			 /// Enable and disable the stream toggle buttons and set their output message if they are disabled.
			 /// </summary>

		/// <summary>
		/// Pick the next source group and start streaming from it.
		/// </summary>
		concurrency::task<void> PickNextMediaSourceAsync();

		/// <summary>
		/// Switch to the next eligible media source.
		/// </summary>
		concurrency::task<void> PickNextMediaSourceWorkerAsync();

		/// <summary>
		/// Creates a reader for the frame source described by the MediaFrameSourceInfo.
		/// On frame arrived will render the output to the supplied software bitmap source.
		/// </summary>
		concurrency::task<void> CreateReaderAsync(Windows::Media::Capture::Frames::MediaFrameSourceInfo^ sourceInfo);

		/// <summary>
		/// Initialize the media capture object.
		/// Must be called from the UI thread.
		/// </summary>
		concurrency::task<bool> TryInitializeMediaCaptureAsync(Windows::Media::Capture::Frames::MediaFrameSourceGroup^ group);

		/// <summary>
		/// Unregisters FrameArrived event handlers, stops and disposes frame readers
		/// and disposes the MediaCapture object.
		/// </summary>
		concurrency::task<void> CleanupMediaCaptureAsync();

		/// <summary>
		/// Handler for frames which arrive from the MediaFrameReader.
		/// Buffers the required frames for rendering and renders based on which sources are enabled and available.
		/// </summary>
		void FrameReader_FrameArrived(
			Windows::Media::Capture::Frames::MediaFrameReader^ sender,
			Windows::Media::Capture::Frames::MediaFrameArrivedEventArgs^ args);

	private: // Private data.

		const UINT16 bufferSize = 10;

		UINT16 captureButtonPressed = 0;
		UINT16 bufferingFrame = 0;
		UINT16 bufferingFrameCounter = bufferSize;

		SDKTemplate::MainPage^ rootPage;

		UINT32 m_selectedSourceGroupIndex = 0;

		Platform::Agile<Windows::Media::Capture::MediaCapture^> m_mediaCapture;

		Microsoft::WRL::Wrappers::SRWLock m_frameLock;

		std::map<Windows::Media::Capture::Frames::MediaFrameSourceKind, FrameSourceState2> m_frameSources;

		std::unique_ptr<FrameRenderer> m_colorFrameRenderer;
		std::unique_ptr<FrameRenderer> m_depthFrameRenderer;
		std::unique_ptr<FrameRenderer> m_infraredFrameRenderer;

		std::unique_ptr<FrameRenderer> m_singleColorFrameRenderer;
		std::unique_ptr<FrameRenderer> m_singleDepthFrameRenderer;
		std::unique_ptr<FrameRenderer> m_singleInfraredFrameRenderer;

		std::unique_ptr<FrameRenderer> m_depthFilterFrameRenderer;
		std::unique_ptr<FrameRenderer> m_depthImageArray[11];

		SDKTemplate::SimpleLogger^ m_logger;

		Platform::Array<Windows::Foundation::Point>^ m_colorSpacePoints;
		Platform::Array<Windows::Foundation::Numerics::float3>^ m_depthSpacePoints;
		Platform::Array<Windows::Foundation::Point>^ m_inDepthSpacePoints;

		UINT32 m_previousBufferWidth = 0;
		UINT32 m_previousBufferHeight = 0;

		private: // private synchronization
			std::mutex m_pointBufferMutex;
	};
}