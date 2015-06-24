namespace Diresharpei
{
    using Windows.UI.Xaml;
    using Windows.UI.Xaml.Controls;

    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private Direlibre.Rendering m_rendering;

        public MainPage()
        {
            this.InitializeComponent();
            this.Loaded += this.OnLoaded;
            this.Unloaded += this.OnUnloaded;
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            m_rendering = new Direlibre.Rendering(this.SwapChainPanel);
        }

        private void OnUnloaded(object sender, RoutedEventArgs e)
        {
            m_rendering.Dispose();
            m_rendering = null;
        }

        private void OnFillClicked(object sender, RoutedEventArgs e)
        {
            m_rendering.Fill();
        }
    }
}
