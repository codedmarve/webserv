#!/usr/bin/php
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Webserv</title>
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-+0n0xVW2eSR5OomGNYDnhzAbDsOXxcvSN1TPprVMTNDbiYZCxYbOOl7+AMvyTG2x" crossorigin="anonymous">
  <link href="style.css" rel="stylesheet">
          <!-- Favicon-->
          <link rel="icon" type="image/x-icon" href="assets/favicon.ico" />
        <!-- Font Awesome icons (free version)-->
        <script src="https://use.fontawesome.com/releases/v6.3.0/js/all.js" crossorigin="anonymous"></script>
        <!-- Simple line icons-->
        <link href="https://cdnjs.cloudflare.com/ajax/libs/simple-line-icons/2.5.5/css/simple-line-icons.min.css" rel="stylesheet" />
        <!-- Google fonts-->
        <link href="https://fonts.googleapis.com/css?family=Source+Sans+Pro:300,400,700,300italic,400italic,700italic" rel="stylesheet" type="text/css" />
        <!-- Core theme CSS (includes Bootstrap)-->
        <link href="css/styles.css" rel="stylesheet" />
</head>

<body>
  <div id="main">
    <content>

      <a class="menu-toggle rounded" href="#"><i class="fas fa-bars"></i></a>
      <nav id="sidebar-wrapper">
        <ul class="sidebar-nav">
          <li class="sidebar-brand"><a href="#page-top">Webserv/1.1</a></li>
          <li class="sidebar-nav-item"><a href="index.php">index.php</a></li>
          <li class="sidebar-nav-item"><a href="/admin">/admin</a></li>
          <li class="sidebar-nav-item"><a href="/data">/data</a></li>
          <li class="sidebar-nav-item"><a href="/autoindex/">/autoindex</a></li>
          <li class="sidebar-nav-item"><a href="#about">About</a></li>
        </ul>
      </nav>
      
      <section class="content-section form-details text-white text-center" id="">
        <?php echo '<h2 class="text-dark">Served by PHP!</h2>'; ?>
            <div class="container px-4 px-lg-5">
                <div class="content-section-heading">
                    <h2 class="text-dark mb-5">The Team</h2>
                </div>
                <div class="row gx-4 gx-lg-5">
                    <div class="col-lg-4 col-md-6 mb-5 mb-lg-0">
                        <span class="service-icon rounded-circle mx-auto mb-3"><img src="https://cdn.intra.42.fr/users/71adf4dd5e947bf89a7499f034b8028b/moduwole.jpg" alt=""></span>
                        <h4><strong>Marvellous Tobi Oduwole</strong></h4>
                        <div class="social-icons">
                            <a href="https://github.com/codedmarve" target="_blank" class="icon">
                              <i class="fab fa-github"></i>
                            </a>
                            <a href="https://www.linkedin.com/in/codedmarve/" target="_blank" class="icon">
                              <i class="fab fa-linkedin"></i>
                            </a>
                        </div>   
                    </div>
                    <div class="col-lg-4 col-md-6 mb-5 mb-lg-0">
                        <span class="service-icon rounded-circle mx-auto mb-3">
                            <img src="https://cdn.intra.42.fr/users/8cace915230482e0b9db2328cb0b21e4/alappas.png" alt="">
                        </span>
                        <h4><strong>Alexandros Angelos Lappas</strong></h4>
                        <div class="social-icons">
                            <a href="https://github.com/AlexLuthor135" target="_blank" class="icon">
                              <i class="fab fa-github"></i>
                            </a>
                            <a href="https://www.linkedin.com/in/alexandros-lappas-18134528b/" target="_blank" class="icon">
                              <i class="fab fa-linkedin"></i>
                            </a>
                          </div>                         
                    </div>
                    <div class="col-lg-4 col-md-6 mb-5 mb-md-0">
                        <span class="service-icon rounded-circle mx-auto mb-3"><img src="https://cdn.intra.42.fr/users/a07511f857f5fb378d45720e60f53744/novsiann.jpg" alt=""></span>
                        <h4><strong>Nikita Ovsiannik</strong></h4>
                        <div class="social-icons">
                            <a href="https://github.com/novsiannn" target="_blank" class="icon">
                              <i class="fab fa-github"></i>
                            </a>
                            <a href="https://www.instagram.com/ovsyannn/" target="_blank" class="icon">
                              <i class="fab fa-instagram"></i>
                            </a>
                        </div>   
                    </div>
                </div>
            </div>
        </section>
       
        <footer class="footer text-center">
            <div class="container px-4 px-lg-5">
                <p class="text-muted small mb-0">Copyright &copy; Webserv 2024</p>
            </div>
        </footer>
        <!-- Scroll to Top Button-->
        <a class="scroll-to-top rounded" href="#page-top"><i class="fas fa-angle-up"></i></a>
        <!-- Bootstrap core JS-->
      </content>
    </div>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/js/bootstrap.bundle.min.js"></script>
    <!-- Core theme JS-->
    <script src="js/scripts.js"></script>
</body>
</html>