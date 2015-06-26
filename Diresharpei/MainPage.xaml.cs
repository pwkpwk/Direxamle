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
            m_rendering.FillRectangle();
        }

        private void OnScale100Clicked(object sender, RoutedEventArgs e)
        {
            this.ScaleTransformation.ScaleX = 1.0;
            this.ScaleTransformation.ScaleY = 1.0;
        }

        private void OnScale150Clicked(object sender, RoutedEventArgs e)
        {
            this.ScaleTransformation.ScaleX = 1.5;
            this.ScaleTransformation.ScaleY = 1.5;
        }

        private void OnScale200Clicked(object sender, RoutedEventArgs e)
        {
            this.ScaleTransformation.ScaleX = 2.0;
            this.ScaleTransformation.ScaleY = 2.0;
        }
    }
}
